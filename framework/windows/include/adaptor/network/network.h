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

#include <string>

#include "context.h"
#include "footstone/hippy_value.h"

namespace hippy {
inline namespace framework {
inline namespace windows {
inline namespace adaptor {

class Network {
 public:
  virtual ~Network() = default;
  virtual bool Initial() = 0;
  virtual void Fetch(const std::shared_ptr<Context>& context, const std::string& url,
                     const footstone::value::HippyValue& request, std::function<void(std::string)> callback) = 0;
  virtual void GetCookie(const std::string& url, std::function<void(footstone::value::HippyValue)> callback) = 0;
  virtual void SetCookie(const footstone::value::HippyValue& request) = 0;
};

}  // namespace adaptor
}  // namespace windows
}  // namespace framework
}  // namespace hippy
