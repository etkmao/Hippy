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

#include "config.h"
#include "dom/dom_manager.h"
#include "driver/driver.h"
#include "footstone/string_view_utils.h"
#include "renderer/tdf/tdf_render_manager.h"
#include "vfs/uri_loader.h"

#ifdef ENABLE_INSPECTOR
#  include "devtools/devtools_data_source.h"
#  include "devtools/vfs/devtools_handler.h"
#endif

namespace hippy {
inline namespace windows {
inline namespace framework {

using DomManager = hippy::dom::DomManager;
using RenderManager = hippy::dom::RenderManager;
using TDFRenderManager = hippy::render::tdf::TDFRenderManager;
using DevtoolsDataSource = hippy::devtools::DevtoolsDataSource;
using UriLoader = hippy::vfs::UriLoader;
using string_view = footstone::stringview::StringViewUtils::string_view;
using ScopeCallBack = hippy::driver::runtime::V8BridgeUtils::RegisterFunction;

class Engine : public std::enable_shared_from_this<Engine> {
 public:
  Engine(std::shared_ptr<hippy::Config>& config);
  ~Engine() = default;

  void Initialize();
  bool RunScriptFromUri(const string_view& uri);
  void SetScopeCallback(ScopeCallBack scope_callback) { scope_callback_ = scope_callback; }
  ScopeCallBack GetScopeCallBack() { return scope_callback_; }
  void LoadInstance(std::string& load_instance_message);
  std::shared_ptr<hippy::Config> GetConfig() { return config_; }
  void ReloadInstance(uint32_t new_root_id);

  static std::string CreateLoadInstanceMessage(uint32_t root_id);
  static std::string CreateRemoteUri(const std::shared_ptr<Config>& config);

 private:
  void CreateDriver();
  void CreateDomManager();
  void CreateTdfRenderManager(const float density);
  void CreateRootNode(const uint32_t root_id, const float density);
  void CreateDevtools();
  void CreateUriLoader();
  void Bind();

 private:
  std::shared_ptr<hippy::Config> config_{nullptr};
  std::shared_ptr<DomManager> dom_manager_{nullptr};
  std::shared_ptr<TDFRenderManager> tdf_render_manager_{nullptr};
  std::shared_ptr<Driver> driver_{nullptr};
  std::shared_ptr<RootNode> root_node_{nullptr};
  std::shared_ptr<UriLoader> uri_loader_{nullptr};
  std::shared_ptr<DevtoolsDataSource> devtools_data_source_{nullptr};
  uint32_t devtools_id_;
  ScopeCallBack scope_callback_;
};

}  // namespace framework
}  // namespace windows
}  // namespace hippy
