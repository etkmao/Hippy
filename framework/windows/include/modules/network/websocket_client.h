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

#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "curl/curl.h"
#include "curl/curl_wrapper.h"
#include "footstone/task_runner.h"
#include "modules/network/websocket_listener.h"

namespace hippy {
inline namespace framework {
inline namespace windows {
inline namespace module {

struct WebsocketResponseHeader {
  std::string ws_accept;
};

struct WebsockeFrame {
  unsigned char fin;
  unsigned char opcode;
  unsigned char mask;
  unsigned long long payload_len;
  unsigned char masking_key[4];
  unsigned char* payload;
};

class WebsocketClient : public std::enable_shared_from_this<WebsocketClient> {
 public:
  WebsocketClient(uint32_t id, std::string url, std::unordered_map<std::string, std::string> extra_headers);
  ~WebsocketClient();
  void Initial(std::shared_ptr<footstone::TaskRunner> task_runner);

  void Connect();
  void Disconnect(const int32_t code, const std::string& reason);
  void Send(const std::string& send_message);
  void Receive();

  void SetEventListener(std::shared_ptr<WebsocketEventListener> listener) { websocket_event_listener_ = listener; }

 private:
  bool CreateSecretKey();

 private:
  std::mutex mutex_;
  bool is_connected_{false};
  uint32_t id_;
  std::string url_;
  struct curl_slist* headers_{nullptr};
  std::unordered_map<std::string, std::string> extra_headers_;
  std::string secret_key_;
  std::unique_ptr<CurlWrapper> curl_wrapper_;
  std::shared_ptr<WebsocketEventListener> websocket_event_listener_;
  std::shared_ptr<footstone::TaskRunner> task_runner_;
};

}  // namespace module
}  // namespace windows
}  // namespace framework
}  // namespace hippy
