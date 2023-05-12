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

#include <WinSock2.h>
#include <wininet.h>

#include <string>
#include <vector>

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

class HttpCookie {
 public:
  HttpCookie() = default;
  ~HttpCookie() = default;

  bool HttpCookie::SetCookies(const std::string& uri, const std::string& cookies, const std::string& expires);
  bool GetCookies(const std::string& uri, std::string& cookies);
};

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
