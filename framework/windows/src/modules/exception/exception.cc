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

#include "modules/exception/exception.h"

#include "adaptor/exception/debug_exception.h"
#include "footstone/logging.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

Exception::Exception() : exception_adaptor_(std::make_shared<hippy::adaptor::DebugException>(true)){};

Exception::Exception(std::shared_ptr<hippy::adaptor::Exception> exception_adaptor)
    : exception_adaptor_(std::move(exception_adaptor)){};

bool Exception::Initial(const std::shared_ptr<Config>& config) {
  auto adaptor = config->GetAdaptor()->GetExceptionAdaptor();
  if (adaptor) {
    exception_adaptor_ = adaptor;
  }
  return true;
}

void Exception::HandleException(const std::string& desc, const std::string& stack) {
  if (exception_adaptor_) exception_adaptor_->HandleException(desc, stack);
  return;
}

void Exception::HandleBackgroundTracing(const std::string& stack) {
  if (exception_adaptor_) exception_adaptor_->HandleBackgroundTracing(stack);
  return;
}

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
