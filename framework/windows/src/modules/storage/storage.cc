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
#include "footstone/logging.h"

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

void Storage::GetItemsValue(const footstone::value::HippyValue& request,
                            std::function<void(const footstone::value::HippyValue&)> success_callback,
                            std::function<void(const footstone::value::HippyValue&)> fail_callback) {
  std::vector<std::string> keys;
  auto ret = ParserRequestKeys(request, keys);
  FOOTSTONE_DCHECK(ret);
  if (ret && storage_adaptor_) {
    storage_adaptor_->GetItemsValue(keys, success_callback, fail_callback);
  }
}

void Storage::SetItemsValue(const footstone::value::HippyValue& request,
                            std::function<void(const footstone::value::HippyValue&)> success_callback,
                            std::function<void(const footstone::value::HippyValue&)> fail_callback) {
  std::unordered_map<std::string, std::string> kvs;
  auto ret = ParserRequestKVs(request, kvs);
  FOOTSTONE_DCHECK(ret);
  if (ret && storage_adaptor_) {
    storage_adaptor_->SetItemsValue(kvs, success_callback, fail_callback);
  }
}

void Storage::GetAllItemsKey(std::function<void(const footstone::value::HippyValue&)> success_callback,
                             std::function<void(const footstone::value::HippyValue&)> fail_callback) {
  if (storage_adaptor_) {
    storage_adaptor_->GetAllItemsKey(success_callback, fail_callback);
  }
}

void Storage::RemoveItems(const footstone::value::HippyValue& request,
                          std::function<void(const footstone::value::HippyValue&)> success_callback,
                          std::function<void(const footstone::value::HippyValue&)> fail_callback) {
  std::vector<std::string> keys;
  auto ret = ParserRequestKeys(request, keys);
  FOOTSTONE_DCHECK(ret);

  if (ret && storage_adaptor_) {
    storage_adaptor_->RemoveItems(keys, success_callback, fail_callback);
  }
}

bool Storage::ParserRequestKeys(const footstone::value::HippyValue& request, std::vector<std::string>& parsed_keys) {
  footstone::value::HippyValue::HippyValueArrayType parameters;
  bool ret = request.ToArray(parameters);
  if (!ret) return false;
  if (parameters.size() != 1) return false;

  footstone::value::HippyValue::HippyValueArrayType keys;
  ret = parameters[0].ToArray(keys);
  if (!ret) return false;
  for (size_t i = 0; i < keys.size(); i++) {
    std::string key;
    ret = keys[0].ToString(key);
    if (!ret) return false;
    parsed_keys.push_back(key);
  }
  return true;
}

bool Storage::ParserRequestKVs(const footstone::value::HippyValue& request,
                               std::unordered_map<std::string, std::string>& parsed_kvs) {
  footstone::value::HippyValue::HippyValueArrayType parameters;
  bool ret = request.ToArray(parameters);
  if (!ret) return false;
  if (parameters.size() != 1) return false;

  footstone::value::HippyValue::HippyValueArrayType kvs;
  ret = parameters[0].ToArray(kvs);
  if (!ret) return false;
  for (size_t i = 0; i < kvs.size(); i++) {
    footstone::value::HippyValue::HippyValueArrayType kv;
    kvs[i].ToArray(kv);
    if (kv.size() != 2) return false;
    std::string key, value;
    ret = kv[0].ToString(key);
    if (!ret) return false;
    ret = kv[1].ToString(value);
    if (!ret) return false;
    parsed_kvs.insert({key, value});
  }
  return true;
}

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
