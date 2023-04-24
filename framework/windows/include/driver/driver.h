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

#include <any>
#include <functional>
#include <memory>
#include <string>

#include "config.h"
#include "context.h"
#include "dom/root_node.h"
#include "driver/base/common.h"
#include "driver/module_dispatcher.h"
#include "driver/napi/v8/v8_ctx.h"
#include "driver/runtime/v8/runtime.h"
#include "driver/vm/v8/v8_vm.h"
#include "footstone/string_view_utils.h"

namespace hippy {
inline namespace framework {
inline namespace windows {

using string_view = footstone::stringview::StringViewUtils::string_view;
using ScopeCallBack = hippy::base::RegisterFunction;
using ExceptionHandler = std::function<void(const std::shared_ptr<hippy::Runtime>& runtime,
                                            const hippy::driver::napi::Ctx::string_view& desc,
                                            const hippy::driver::napi::Ctx::string_view& stack)>;

class Driver : public std::enable_shared_from_this<Driver> {
 public:
  Driver();
  ~Driver() = default;

  void SetConfig(std::shared_ptr<Config> config) { config_ = config; }
  void SetScopeCallBack(ScopeCallBack scope_callback) { scope_callback_ = scope_callback; };
  void SetExceptionHandler(ExceptionHandler exception_handler) { exception_handler_ = exception_handler; }

  bool Initialize(std::shared_ptr<Context> context, std::shared_ptr<DomManager> dom_manager,
                  std::shared_ptr<RootNode> root_node, uint32_t devtools_id);
  bool RunScriptFromUri(string_view uri, std::shared_ptr<hippy::vfs::UriLoader> uri_loader);
  void LoadInstance(uint32_t root_id);

 private:
  int32_t runtime_id_;
  std::shared_ptr<Config> config_;
  std::shared_ptr<hippy::V8VMInitParam> v8_init_param_;
  ScopeCallBack scope_callback_;
  ExceptionHandler exception_handler_;
  std::shared_ptr<hippy::ModuleDispatcher> module_dispatcher_;
};

}  // namespace windows
}  // namespace framework
}  // namespace hippy
