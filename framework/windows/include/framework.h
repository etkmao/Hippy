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
#include <unordered_map>

#include "adaptor/console/console.h"
#include "config.h"
#include "context.h"
#include "dom/dom_manager.h"
#include "driver/driver.h"
#include "driver/module_dispatcher.h"
#include "renderer/tdf/tdf_render_manager.h"

namespace hippy {
inline namespace framework {
inline namespace windows {

class FrameWork : public std::enable_shared_from_this<FrameWork> {
 public:
  FrameWork();
  ~FrameWork() = default;
  bool Initialize(std::shared_ptr<Config> config);
  void SetContext(std::shared_ptr<hippy::Context> context) { context_ = context; }

  std::shared_ptr<hippy::DomManager> GetDomManager() { return dom_manager_; }
  uint32_t CreateDevtools();
  std::shared_ptr<hippy::TDFRenderManager> GetRenderManager() { return render_manager_; }

  bool RunScriptFromUri(footstone::stringview::StringViewUtils::string_view uri);
  void LoadInstance();

 private:
  void CreateDomManager();
  void CreateRenderManager();
  void CreateDriver();

 private:
  std::shared_ptr<hippy::Context> context_{nullptr};
  std::shared_ptr<hippy::DomManager> dom_manager_{nullptr};
  std::shared_ptr<hippy::TDFRenderManager> render_manager_{nullptr};
  std::shared_ptr<hippy::Driver> driver_{nullptr};
  std::shared_ptr<hippy::Console> console_adaptor_{nullptr};
  uint32_t initial_heap_size_in_bytes_;
  uint32_t maximum_heap_size_in_bytes_;
  int32_t runtime_id_;
};

}  // namespace windows
}  // namespace framework
}  // namespace hippy
