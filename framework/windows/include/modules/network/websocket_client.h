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

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "footstone/logging.h"
#include "modules/network/websocket_listener.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wtautological-type-limit-compare"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wunused-private-field"
#pragma clang diagnostic ignored "-Wsuggest-override"
#define ASIO_STANDALONE
#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_no_tls_client.hpp"
#pragma clang diagnostic pop

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

using WSClient = websocketpp::client<websocketpp::config::asio_client>;
using WSConnectionHandle = websocketpp::connection_hdl;
using WSMessagePtr = websocketpp::config::asio_client::message_type::ptr;
using WSThread = websocketpp::lib::shared_ptr<websocketpp::lib::thread>;

class WebsocketClient : public std::enable_shared_from_this<WebsocketClient> {
 public:
  WebsocketClient(uint32_t id, std::string ws_url, std::unordered_map<std::string, std::string> extra_headers);
  ~WebsocketClient() = default;
  void Connect();
  void Disconnect(const int32_t code, const std::string& reason);
  void Send(const std::string& send_message);
  void SetEventListener(std::shared_ptr<WebsocketEventListener> listener) { ws_event_listener_ = listener; }
  void SetCloseCallback(std::function<void(uint32_t)> callback) { callback_ = callback; }

 private:
  void HandleSocketInit(const websocketpp::connection_hdl& handle);
  void HandleSocketConnectFail(const websocketpp::connection_hdl& handle);
  void HandleSocketConnectOpen(const websocketpp::connection_hdl& handle);
  void HandleSocketConnectMessage(const websocketpp::connection_hdl& handle, const WSMessagePtr& message_ptr);
  void HandleSocketConnectClose(const websocketpp::connection_hdl& handle);

 private:
  uint32_t id_;
  std::string ws_url_;
  std::unordered_map<std::string, std::string> extra_headers_;
  WSClient ws_client_;
  WSThread ws_thread_;
  WSConnectionHandle ws_connection_handle_;
  std::vector<std::string> unset_messages_{};
  std::shared_ptr<WebsocketEventListener> ws_event_listener_;
  std::function<void(uint32_t)> callback_;
};

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
