/*
 *
 * Tencent is pleased to support the open source community by making
 * Hippy available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "modules/network/websocket_client.h"

#include <string.h>
#include <wincrypt.h>
#include <iomanip>
#include <thread>

#include "curl/curl_wrapper.h"
#include "footstone/cv_driver.h"
#include "footstone/logging.h"
#include "footstone/macros.h"
#include "footstone/task_runner.h"
#include "footstone/worker_impl.h"

namespace hippy {
inline namespace framework {
inline namespace windows {
inline namespace module {

WebsocketClient::WebsocketClient(uint32_t id, std::string url,
                                 std::unordered_map<std::string, std::string> extra_headers)
    : id_(id), url_(url), extra_headers_(extra_headers), curl_wrapper_(std::make_unique<CurlWrapper>()) {
}

WebsocketClient::~WebsocketClient() { curl_slist_free_all(headers_); }

void WebsocketClient::Initial(std::shared_ptr<footstone::TaskRunner> task_runner) { task_runner_ = task_runner; }

// TODO(charleeshen): 是否需要检查 secrect key 正确
static size_t WebsocketCurlWriteHeader(char* buffer, size_t size, size_t nitems, void* userdata) {
  struct WebsocketResponseHeader* resp_headers = (struct WebsocketResponseHeader*)userdata;
  char* field = "Sec-WebSocket-Accept: ";
  if (strncmp(buffer, field, strlen(field)) == 0) {
    resp_headers->ws_accept = std::string(buffer + strlen(field), nitems - strlen(field) - 2);
  }
  return nitems;
}

bool WebsocketClient::CreateSecretKey() {
  char key[16];
  srand(time(NULL));
  for (int i = 0; i < 16; i++) {
    key[i] = rand() % 256;
  }
  DWORD size = 0;
  if (!CryptBinaryToStringA(reinterpret_cast<BYTE*>(key), (DWORD)16, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr,
                            &size)) {
    return false;
  }
  secret_key_.resize(size);
  if (!CryptBinaryToStringA(reinterpret_cast<BYTE*>(key), (DWORD)16, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
                            &secret_key_[0], &size)) {
    return false;
  }
  return true;
}

void WebsocketClient::Connect() {
  {
    std::lock_guard<std::mutex> lock_gurad(mutex_);
    if (is_connected_) return;
  }

  auto handshake = [WEAK_THIS]() {
    DEFINE_AND_CHECK_SELF(WebsocketClient)

    self->curl_wrapper_->Initialize();
    auto curl = self->curl_wrapper_->GetCurlRawPointer();
    auto url = self->url_;
    struct curl_slist* headers = self->headers_;
    for (const auto& kv : self->extra_headers_) {
      std::string h = kv.first + ": " + kv.second;
      headers = curl_slist_append(headers, h.c_str());
    }
    struct WebsocketResponseHeader resp_headers;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 2L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    if (headers != nullptr) {
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WebsocketCurlWriteHeader);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &resp_headers);

    auto res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      FOOTSTONE_DLOG(INFO) << "curl_easy_perform() failed: " << curl_easy_strerror(res);
      return;
    }
    self->is_connected_ = true;
    self->Receive();
    if (self->websocket_event_listener_) self->websocket_event_listener_->Open();
  };

  task_runner_->PostTask(handshake);
  return;
}

// TODO(charleeshen): 数据量大的时候可能需要分片
void WebsocketClient::Send(const std::string& send_message) {
  auto send_frame = [WEAK_THIS, send_message]() {
    DEFINE_AND_CHECK_SELF(WebsocketClient)

    size_t send;
    auto curl = self->curl_wrapper_->GetCurlRawPointer();
    auto url = self->url_;
    auto res = curl_ws_send(curl, send_message.c_str(), send_message.length(), &send, 0, CURLWS_BINARY);
    if (res != CURLE_OK) {
      FOOTSTONE_DLOG(INFO) << "curl_ws_send() failed: " << curl_easy_strerror(res);
      if (self->websocket_event_listener_) self->websocket_event_listener_->Error(curl_easy_strerror(res));
      return;
    }
    self->Receive();

    return;
  };
  task_runner_->PostTask(send_frame);
}

void WebsocketClient::Receive() {
  auto recv_frame = [WEAK_THIS]() {
    DEFINE_AND_CHECK_SELF(WebsocketClient)

    std::string recv_message;
    auto curl = self->curl_wrapper_->GetCurlRawPointer();
    size_t i = 0;
    size_t nread = 0;
    struct curl_ws_frame* meta;
    char buffer[256];
    while (true) {
      auto res = curl_ws_recv(curl, buffer, sizeof(buffer), &nread, &meta);
      if (res == CURLE_AGAIN) {
        FOOTSTONE_DLOG(INFO) << "curl_ws_recv() continue";
        recv_message += std::string(buffer, meta->len);
        continue;
      } else if (res == CURLE_OK) {
        recv_message += std::string(buffer, meta->len);
        if (self->websocket_event_listener_) self->websocket_event_listener_->Message(recv_message);
        return;
      } else {
        if (self->websocket_event_listener_) self->websocket_event_listener_->Error(curl_easy_strerror(res));
        return;
      }
    }
  };
  task_runner_->PostTask(recv_frame);
}

// TODO(charleeshen): 改用 frame 传递数据， 传递 code 和 reason 给服务器
void WebsocketClient::Disconnect(const int32_t code, const std::string& reason) {
  auto disconnect = [WEAK_THIS]() {
    DEFINE_AND_CHECK_SELF(WebsocketClient)
    // clean up websocket
    auto curl = self->curl_wrapper_->GetCurlRawPointer();
    self->curl_wrapper_->Cleanup();
    if (self->websocket_event_listener_) self->websocket_event_listener_->Close(0, "closed");
  };
  task_runner_->PostTask(disconnect);

}

}  // namespace module
}  // namespace windows
}  // namespace framework
}  // namespace hippy
