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

#include "dom/root_node.h"
#include "vfs/vfs_manager.h"

namespace hippy {
inline namespace framework {
inline namespace windows {

class Context {
 public:
  Context() = default;
  ~Context() = default;

  float GetDensity() { return density_; }
  void SetDensity(float density) { density_ = density; }
  void SetRuntimeId(int32_t runtime_id) { runtime_id_ = runtime_id; }
  int32_t GetRuntimeId() { return runtime_id_; }
  void CreateRootNode();
  std::shared_ptr<RootNode> GetRootNode();
  int32_t GetRootNodeId() { return root_id_; }
  void CreateVfsManager() { vfs_manager_ = std::make_shared<hippy::VfsManager>(); }
  std::shared_ptr<VfsManager> GetVfsManager() { return vfs_manager_; };
  void CreateUriLoader();
  std::shared_ptr<UriLoader> GetUriLoader();

 private:
  float density_;
  int32_t root_id_;
  int32_t runtime_id_;
  uint32_t uri_loader_id_;
  std::shared_ptr<VfsManager> vfs_manager_{nullptr};
};

}  // namespace windows
}  // namespace framework
}  // namespace hippy
