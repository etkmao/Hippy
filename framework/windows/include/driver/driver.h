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

#include "config.h"
#include "dom/dom_manager.h"
#include "driver/js_driver_utils.h"
#include "driver/module_dispatcher.h"
#include "footstone/serializer.h"

namespace hippy {
inline namespace windows {

using string_view = footstone::stringview::string_view;
using ScopeInitializedCallBack = std::function<void(std::shared_ptr<Scope>)>;
using ExceptionHandler = std::function<void(const string_view& desc, const string_view& stack)>;
using Serializer = footstone::value::Serializer;
using DevtoolsDataSource = hippy::devtools::DevtoolsDataSource;

class Driver : public std::enable_shared_from_this<Driver> {
 public:
  Driver();
  ~Driver() = default;

  bool Initialize(const std::shared_ptr<Config>& config, const std::shared_ptr<DomManager>& dom_manager,
                  const std::shared_ptr<RootNode>& root_node, const std::shared_ptr<UriLoader>& uri_loader,
                  const std::shared_ptr<DevtoolsDataSource>& devtools_data_source, bool reload = false);
  void SetScope(std::shared_ptr<Scope> scope) { scope_ = scope; }
  std::shared_ptr<Scope> GetScope() { return scope_; }

  // void RegisterExceptionHandler();
  void SetExceptionHandler(ExceptionHandler exception_handler) { exception_handler_ = exception_handler; }
  void ScopeInitializedCallBack(ScopeInitializedCallBack scope_initialized_callback) { scope_initialized_callback_ = scope_initialized_callback; }
  hippy::ScopeInitializedCallBack GetScopeInitializedCallBack() { return scope_initialized_callback_; }
  bool RunScriptFromUri(string_view uri, const std::shared_ptr<UriLoader>& uri_loader,
                        const std::shared_ptr<Config>& config);
  void LoadInstance(std::string& load_instance_message);

  void ReloadInstance(const uint32_t root_id, std::function<void()> reload_callback);

 private:
  std::shared_ptr<Scope> scope_;
  std::shared_ptr<Engine> js_engine_;
  std::shared_ptr<hippy::ModuleDispatcher> module_dispatcher_;
  ExceptionHandler exception_handler_;
  hippy::ScopeInitializedCallBack scope_initialized_callback_;
  Serializer serializer_;
};

}  // namespace windows
}  // namespace hippy
