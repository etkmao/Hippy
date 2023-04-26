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

#include "footstone/hippy_value.h"

#include <functional>

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

class Clipboard {
 public:
  Clipboard() = default;
  ~Clipboard() = default;

  bool Initial();
  void GetString(std::function<void(footstone::value::HippyValue)> callback);
  void SetString(const footstone::value::HippyValue& buffer);

 private:
  bool ParserRequest(const footstone::value::HippyValue& value, std::string& clipboard_string);
};

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
