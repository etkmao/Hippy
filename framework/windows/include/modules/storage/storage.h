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
#include <string>
#include <unordered_map>
#include <vector>

#include "adaptor/storage/storage.h"
#include "footstone/hippy_value.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

class Storage {
 public:
  Storage();
  Storage(std::shared_ptr<hippy::adaptor::Storage> storage_adaptor);
  ~Storage() = default;

  bool Initial();

  void GetItemsValue(const footstone::value::HippyValue& request,
                     std::function<void(const footstone::value::HippyValue&)> success_callback,
                     std::function<void(const footstone::value::HippyValue&)> fail_callback);
  void SetItemsValue(const footstone::value::HippyValue& request,
                     std::function<void(const footstone::value::HippyValue&)> success_callback,
                     std::function<void(const footstone::value::HippyValue&)> fail_callback);
  void RemoveItems(const footstone::value::HippyValue& request,
                   std::function<void(const footstone::value::HippyValue&)> success_callback,
                   std::function<void(const footstone::value::HippyValue&)> fail_callback);
  void GetAllItemsKey(std::function<void(const footstone::value::HippyValue&)> success_callback,
                      std::function<void(const footstone::value::HippyValue&)> fail_callback);

 private:
  bool ParserRequestKeys(const footstone::value::HippyValue& request, std::vector<std::string>& parsed_keys);

  bool ParserRequestKVs(const footstone::value::HippyValue& request,
                        std::unordered_map<std::string, std::string>& parsed_kvs);

 private:
  std::shared_ptr<hippy::adaptor::Storage> storage_adaptor_;
};

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
