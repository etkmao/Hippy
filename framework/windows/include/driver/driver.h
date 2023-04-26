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

#include "config.h"
#include "dom/dom_manager.h"
#include "driver/module_dispatcher.h"

namespace hippy {
inline namespace windows {

using ScopeCallBack = hippy::driver::runtime::V8BridgeUtils::RegisterFunction;
using ExceptionHandler = std::function<void(const std::shared_ptr<hippy::Runtime>& runtime,
                                            const hippy::driver::napi::Ctx::string_view& desc,
                                            const hippy::driver::napi::Ctx::string_view& stack)>;

class Driver : public std::enable_shared_from_this<Driver> {
 public:
  Driver();
  ~Driver() = default;

  bool Initialize(const std::shared_ptr<Config>& config, const std::shared_ptr<DomManager>& dom_manager,
                  const std::shared_ptr<RootNode>& root_node, const std::shared_ptr<UriLoader>& uri_loader,
                  const uint32_t devtools_id);

  void RegisterExceptionHandler();
  void SetExceptionHandler(ExceptionHandler exception_handler) { exception_handler_ = exception_handler; }
  void SetScopeCallBack(ScopeCallBack scope_callback) { scope_callback_ = scope_callback; }
  ScopeCallBack GetScopeCallBack() { return scope_callback_; }
  void LoadInstance(std::string& load_instance_message);
  bool RunScriptFromUri(string_view uri, const std::shared_ptr<UriLoader>& uri_loader,
                        const std::shared_ptr<Config>& config);

 private:
  int32_t runtime_id_;
  std::shared_ptr<hippy::ModuleDispatcher> module_dispatcher_;
  ExceptionHandler exception_handler_;
  ScopeCallBack scope_callback_;
};

}  // namespace windows
}  // namespace hippy
