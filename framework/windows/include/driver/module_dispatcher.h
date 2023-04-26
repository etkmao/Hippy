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

#include <string>
#include <unordered_map>
#include <vector>

#include "driver/runtime/v8/runtime.h"
#include "driver/runtime/v8/v8_bridge_utils.h"
#include "modules/clipboard/clipboard.h"
#include "modules/netinfo/net_info.h"
#include "modules/network/network.h"
#include "modules/network/websocket.h"
#include "modules/storage/storage.h"

namespace hippy {
inline namespace windows {
inline namespace framework {

using string_view = footstone::stringview::string_view;
using byte_string = std::string;

class ModuleDispatcher : public std::enable_shared_from_this<ModuleDispatcher> {
 public:
  ModuleDispatcher();
  void Initial();
  void ModuleDispatcher::Dispatcher(const CallbackInfo& info, const std::shared_ptr<Runtime>& runtime);

 private:
  bool ParserParameters(const footstone::value::HippyValue& value,
                        std::unordered_map<std::string, std::string>& parsed);
  bool ParserParameters(const footstone::value::HippyValue& value, std::vector<std::string>& parsed);

  void StorageModuleHandle(const string_view& func, int32_t runtime_id, const string_view& cb_id,
                           const footstone::value::HippyValue& buffer);
  void WebsocketModuleHandle(const string_view& func, int32_t runtime_id, const string_view& cb_id,
                             const footstone::value::HippyValue& buffer);
  void NetInfoModuleHandle(const string_view& func, int32_t runtime_id, const string_view& cb_id,
                           const footstone::value::HippyValue& buffer);
  void NetworkModuleHandle(const std::shared_ptr<UriLoader>& uri_loader, const string_view& func, int32_t runtime_id,
                           const string_view& cb_id, const footstone::value::HippyValue& buffer);
  void ClipboardModuleHandle(const string_view& func, int32_t runtime_id, const string_view& cb_id,
                             const footstone::value::HippyValue& buffer);

 private:
  std::shared_ptr<hippy::windows::framework::module::Storage> storage_module_;      // storage module
  std::shared_ptr<hippy::windows::framework::module::Websocket> websocket_module_;    // websocket module
  std::shared_ptr<hippy::windows::framework::module::NetInfo> net_info_module_;     // net info module
  std::shared_ptr<hippy::windows::framework::module::Network> network_module_;        // network module
  std::shared_ptr<hippy::windows::framework::module::Clipboard> clipboard_module_;  // clipboard module
};

}  // namespace framework
}  // namespace windows
}  // namespace hippy
