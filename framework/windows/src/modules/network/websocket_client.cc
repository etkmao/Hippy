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

#include "footstone/cv_driver.h"
#include "footstone/logging.h"
#include "footstone/macros.h"
#include "footstone/task_runner.h"
#include "footstone/worker_impl.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

WebsocketClient::WebsocketClient(uint32_t id, std::string ws_url,
                                 std::unordered_map<std::string, std::string> extra_headers)
    : id_(id), ws_url_(ws_url), extra_headers_(extra_headers) {
  ws_client_.clear_access_channels(websocketpp::log::alevel::all);
  ws_client_.set_access_channels(websocketpp::log::alevel::fail);
  ws_client_.set_error_channels(websocketpp::log::elevel::all);
  // Initialize ASIO
  // for (const auto& [key, value] : extra_headers_) {
  //  ws_client_.set_message_header(key, value);
  // }
  websocketpp::lib::error_code error_code;
  ws_client_.init_asio(error_code);
  ws_client_.start_perpetual();
}

void WebsocketClient::Connect() {
  if (ws_url_.empty()) {
    FOOTSTONE_DLOG(INFO) << "websocket uri is empty, connect error";
    return;
  }
  ws_client_.set_socket_init_handler(
      [WEAK_THIS](const websocketpp::connection_hdl& handle, websocketpp::lib::asio::ip::tcp::socket& socket) {
        DEFINE_AND_CHECK_SELF(WebsocketClient)
        self->HandleSocketInit(handle);
      });
  ws_client_.set_open_handler([WEAK_THIS](const websocketpp::connection_hdl& handle) {
    DEFINE_AND_CHECK_SELF(WebsocketClient)
    self->HandleSocketConnectOpen(handle);
  });
  ws_client_.set_close_handler([WEAK_THIS](const websocketpp::connection_hdl& handle) {
    DEFINE_AND_CHECK_SELF(WebsocketClient)
    self->HandleSocketConnectClose(handle);
    if (self->callback_) self->callback_(self->id_);
  });
  ws_client_.set_fail_handler([WEAK_THIS](const websocketpp::connection_hdl& handle) {
    DEFINE_AND_CHECK_SELF(WebsocketClient)
    self->HandleSocketConnectFail(handle);
  });
  ws_client_.set_message_handler(
      [WEAK_THIS](const websocketpp::connection_hdl& handle, const WSMessagePtr& message_ptr) {
        DEFINE_AND_CHECK_SELF(WebsocketClient)
        self->HandleSocketConnectMessage(handle, message_ptr);
      });

  ws_thread_ = websocketpp::lib::make_shared<websocketpp::lib::thread>(&WSClient::run, &ws_client_);
  ws_thread_->detach();
  websocketpp::lib::error_code error_code;
  auto con = ws_client_.get_connection(ws_url_, error_code);
  if (error_code) {
    ws_client_.get_alog().write(websocketpp::log::alevel::app, error_code.message());
    return;
  }
  ws_client_.connect(con);
}

void WebsocketClient::Send(const std::string& message) {
  if (!ws_connection_handle_.lock()) {
    unset_messages_.emplace_back(message);
    return;
  }
  websocketpp::lib::error_code error_code;
  ws_client_.send(ws_connection_handle_, message, websocketpp::frame::opcode::text, error_code);
}

void WebsocketClient::Disconnect(const int32_t code, const std::string& reason) {
  ws_client_.stop_perpetual();
  if (!ws_connection_handle_.lock()) {
    FOOTSTONE_DLOG(INFO) << "send message error, handler is null";
    return;
  }
  FOOTSTONE_DLOG(INFO) << "close websocket, code: " << code << ", reason: " << reason.c_str();
  websocketpp::lib::error_code error_code;
  ws_client_.close(ws_connection_handle_, static_cast<websocketpp::close::status::value>(code), reason, error_code);
}

void WebsocketClient::HandleSocketInit(const websocketpp::connection_hdl& handle) {
  FOOTSTONE_DLOG(INFO) << "websocket init";
}

void WebsocketClient::HandleSocketConnectFail(const websocketpp::connection_hdl& handle) {
  websocketpp::lib::error_code error_code;
  auto con = ws_client_.get_con_from_hdl(handle, error_code);
  unset_messages_.clear();
  FOOTSTONE_DLOG(INFO) << "websocket connect fail, state: " << con->get_state()
                       << ", error message:" << con->get_ec().message().c_str()
                       << ", local close code:" << con->get_local_close_code()
                       << ", local close reason: " << con->get_local_close_reason().c_str()
                       << ", remote close code:" << con->get_remote_close_code()
                       << ", remote close reason:" << con->get_remote_close_reason().c_str();
}

void WebsocketClient::HandleSocketConnectOpen(const websocketpp::connection_hdl& handle) {
  ws_connection_handle_ = handle.lock();
  FOOTSTONE_DLOG(INFO) << "websocket connect open";
  if (ws_event_listener_) ws_event_listener_->Open();
  if (!ws_connection_handle_.lock() || unset_messages_.empty()) {
    return;
  }
  for (auto& message : unset_messages_) {
    websocketpp::lib::error_code error_code;
    ws_client_.send(ws_connection_handle_, message, websocketpp::frame::opcode::text, error_code);
  }
  unset_messages_.clear();
}

void WebsocketClient::HandleSocketConnectMessage(const websocketpp::connection_hdl& handle,
                                                 const WSMessagePtr& message_ptr) {
  auto message = message_ptr->get_payload();
  std::string data(message.c_str(), message.length());
  if (ws_event_listener_) ws_event_listener_->Message(data);
}

void WebsocketClient::HandleSocketConnectClose(const websocketpp::connection_hdl& handle) {
  websocketpp::lib::error_code error_code;
  auto con = ws_client_.get_con_from_hdl(handle, error_code);
  if (ws_event_listener_) ws_event_listener_->Close(0, "closed");
  unset_messages_.clear();
  FOOTSTONE_DLOG(INFO) << "websocket connect close, state: " << con->get_state()
                       << ", error message:" << con->get_ec().message().c_str()
                       << ", local close code:" << con->get_local_close_code()
                       << ", local close reason: " << con->get_local_close_reason().c_str()
                       << ", remote close code:" << con->get_remote_close_code()
                       << ", remote close reason:" << con->get_remote_close_reason().c_str();
}

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
