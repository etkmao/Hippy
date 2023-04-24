/*
 *
 * Tencent is pleased to support the open source community by making
 * Hippy available.
 *
 * Copyright (C) 2022 THL A29 Limited, a Tencent company.
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

#include "driver/napi/js_try_catch.h"

#include "footstone/string_view.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#pragma clang diagnostic ignored "-Wvla-extension"
#pragma clang diagnostic ignored "-Wundefined-reinterpret-cast"
#pragma clang diagnostic ignored "-Wcast-function-type"
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "v8/v8.h"
#pragma clang diagnostic pop

namespace hippy {
inline namespace driver {
inline namespace napi {

class V8TryCatch : public TryCatch {
 public:
  explicit V8TryCatch(bool enable = false, const std::shared_ptr<Ctx>& ctx = nullptr);
  virtual ~V8TryCatch() override;

  virtual void ReThrow() override;
  virtual bool HasCaught() override;
  virtual bool CanContinue() override;
  virtual bool HasTerminated() override;
  virtual bool IsVerbose() override;
  virtual void SetVerbose(bool verbose) override;
  virtual std::shared_ptr<CtxValue> Exception() override;
  virtual footstone::string_view GetExceptionMessage() override;

 private:
  std::shared_ptr<v8::TryCatch> try_catch_;
};

}
}
}
