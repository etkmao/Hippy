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

#include "vfs/uri_loader.h"

namespace hippy {
inline namespace framework {
inline namespace windows {

class VfsManager {
 public:
  VfsManager() = default;
  ~VfsManager() = default;
  uint32_t CreateUriLoader();
  std::shared_ptr<hippy::vfs::UriLoader> GetUriLoader(uint32_t id);

 private:
  std::unordered_map<uint32_t, std::shared_ptr<hippy::vfs::UriLoader>> uri_loader_map_;
};

}  // namespace windows
}  // namespace framework
}  // namespace hippy
