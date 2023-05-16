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

#include "adaptor/exception/exception.h"
#include "config.h"
#include "footstone/hippy_value.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

class Exception {
 public:
  Exception();
  Exception(std::shared_ptr<hippy::adaptor::Exception> exception_adaptor);
  ~Exception() = default;

  bool Initial(const std::shared_ptr<Config>& config);
  void HandleException(const std::string& desc, const std::string& stack);
  void HandleBackgroundTracing(const std::string& stack);

 private:
  std::shared_ptr<hippy::adaptor::Exception> exception_adaptor_;
};

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
