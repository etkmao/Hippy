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

#include <Windows.h>
#include <Comdef.h>
#include <Gdiplus.h>
#include <functional>

#include "footstone/hippy_value.h"
#include "vfs/uri_loader.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

class ImageLoader {
 public:
  ImageLoader() = default;
  ~ImageLoader();

  bool Initial();
  void GetSize(const std::shared_ptr<UriLoader>& uri_loader, const footstone::value::HippyValue& request,
               std::function<void(const footstone::HippyValue& callback_parameter)> callback);
  void Prefetch();

 private:
  bool ParserRequestUrl(const footstone::value::HippyValue& request, std::string& url);
  ULONG_PTR gdiplus_token_;
};

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
