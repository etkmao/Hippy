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

#include "adaptor/storage/storage.h"

namespace hippy {
inline namespace framework {
inline namespace windows {
inline namespace module {

using StorageResponse = hippy::framework::windows::module::StorageResponse;

class Storage {
 public:
  Storage();
  Storage(std::shared_ptr<hippy::adaptor::Storage> storage_adaptor);
  ~Storage() = default;

  bool Initial();

  void GetItemsValue(std::vector<std::string> keys,
                     std::function<void(StorageResponse, std::unordered_map<std::string, std::string>)> callback);
  void SetItemsValue(std::unordered_map<std::string, std::string> kvs, std::function<void(StorageResponse)> callback);
  void RemoveItems(std::vector<std::string> keys, std::function<void(StorageResponse)> callback);
  void GetAllItemsKey(std::function<void(StorageResponse, std::vector<std::string>)> callback);

 private:
  std::shared_ptr<hippy::adaptor::Storage> storage_adaptor_;
};

}  // namespace module
}  // namespace windows
}  // namespace framework
}  // namespace hippy