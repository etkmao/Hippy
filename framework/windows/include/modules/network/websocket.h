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
#include <unordered_map>

#include "footstone/hippy_value.h"
#include "modules/network/websocket_client.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

class Websocket : public std::enable_shared_from_this<Websocket> {
 public:
  Websocket() = default;
  ~Websocket();

  void Initial();
  void Connect(const footstone::value::HippyValue& request, uint32_t runtime_id,
               std::function<void(footstone::value::HippyValue)> callback);
  void Disconnect(const footstone::value::HippyValue& request, uint32_t runtime_id);
  void Send(const footstone::value::HippyValue& request, uint32_t runtime_id);

 private:
  enum class RetCode { Success, Failed, UrlError, HeaderError };

 private:
  RetCode ParseRequest(const footstone::value::HippyValue& request, std::string& url,
                       std::unordered_map<std::string, std::string>& headers);
  RetCode ParseRequest(const footstone::value::HippyValue& request, int32_t& websocket_id, std::string& data);

  RetCode ParseRequest(const footstone::value::HippyValue& request, int32_t& websocket_id, int32_t& code,
                       std::string& reason);

 private:
  std::unordered_map<std::uint32_t, std::shared_ptr<WebsocketClient>> websocket_client_map_;
};

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
