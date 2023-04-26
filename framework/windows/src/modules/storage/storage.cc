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

#include "modules/storage/storage.h"
#include "adaptor/storage/sqlite_storage.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

Storage::Storage() : storage_adaptor_(std::make_shared<adaptor::SqliteStorage>()){};

Storage::Storage(std::shared_ptr<adaptor::Storage> storage_adaptor) : storage_adaptor_(std::move(storage_adaptor)){};

bool Storage::Initial() {
  if (storage_adaptor_) return storage_adaptor_->Initial();
  return false;
}

void Storage::GetItemsValue(
    std::vector<std::string> keys,
    std::function<void(StorageResponse, std::unordered_map<std::string, std::string>)> callback) {
  if (storage_adaptor_) {
    storage_adaptor_->GetItemsValue(keys, callback);
  }
}
void Storage::SetItemsValue(std::unordered_map<std::string, std::string> kvs,
                            std::function<void(StorageResponse)> callback) {
  if (storage_adaptor_) {
    storage_adaptor_->SetItemsValue(kvs, callback);
  }
}

void Storage::GetAllItemsKey(std::function<void(StorageResponse, std::vector<std::string>)> callback) {
  if (storage_adaptor_) {
    storage_adaptor_->GetAllItemsKey(callback);
  }
}
void Storage::RemoveItems(std::vector<std::string> keys, std::function<void(StorageResponse)> callback) {
  if (storage_adaptor_) {
    storage_adaptor_->RemoveItems(keys, callback);
  }
}

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
