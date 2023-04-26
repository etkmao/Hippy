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
#include <string>
#include <unordered_map>
#include <vector>

#include "modules/storage/storage_response.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

using StorageResponse = hippy::module::StorageResponse;

class Storage {
 public:
  virtual ~Storage() = default;
  virtual bool Initial() = 0;
  virtual void GetItemsValue(
      std::vector<std::string> keys,
      std::function<void(StorageResponse, std::unordered_map<std::string, std::string>)> callback) = 0;
  virtual void SetItemsValue(std::unordered_map<std::string, std::string> kvs,
                             std::function<void(StorageResponse)> callback) = 0;
  virtual void GetAllItemsKey(std::function<void(StorageResponse, std::vector<std::string>)> callback) = 0;
  virtual void RemoveItems(std::vector<std::string> keys, std::function<void(StorageResponse)> callback) = 0;
};

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
