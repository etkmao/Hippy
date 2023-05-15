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

#include "driver/scope.h"
#include "footstone/hippy_value.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

class WebsocketEventListener {
 public:
  WebsocketEventListener(const std::shared_ptr<Scope>& scope, uint32_t websocket_client_id);
  void Open();
  void Close(const int32_t code, const std::string& reason);
  void Error(const std::string& error_message);
  void Message(const std::string& recv_message);

 private:
  void SendWebsocketEvent(std::string event_type, footstone::HippyValue data);
  std::weak_ptr<Scope> weak_scope_;
  uint32_t websocket_client_id_;
};

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
