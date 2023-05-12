/*
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
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "adaptor/network/http_cookie.h"

#include <WinSock2.h>
#include <wininet.h>

#include <regex>
#include <sstream>

#include "footstone/logging.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

static std::vector<std::string> Split(const std::string& split_string, char delim) {
  std::vector<std::string> vec;
  std::stringstream ss(split_string);
  std::string temp;
  while (std::getline(ss, temp, delim)) {
    vec.push_back(temp);
  }
  return vec;
}

bool HttpCookie::SetCookies(const std::string& uri, const std::string& cookies, const std::string& expires) {
  std::regex reg("\\s+");
  std::string cookies_normalized = regex_replace(cookies, reg, "");
  std::vector<std::string> cookie_vector;
  cookie_vector = Split(cookies_normalized, ';');
  bool ret = true;
  for (auto& s : cookie_vector) {
    auto kv = Split(s, '=');
    FOOTSTONE_DCHECK(kv.size() == 1 || kv.size() == 2);
    if (kv.size() == 1 || kv.size() == 2) {
      if (kv.size() == 1 || kv.size() >= 2 && kv[1].empty()) {
        ret = InternetSetCookie(uri.c_str(), kv[0].c_str(), ";Max-Age=0");
      } else {
        if (expires.empty()) {
          ret = InternetSetCookie(uri.c_str(), kv[0].c_str(), kv[1].c_str());
        } else {
          std::string value = kv[1] + ";expires=" + expires;
          ret = InternetSetCookie(uri.c_str(), kv[0].c_str(), value.c_str());
        }
      }
      if (!ret) FOOTSTONE_DLOG(INFO) << " insert cookie error [%s]" << cookies;
    }
  }
  return ret;
}

bool HttpCookie::GetCookies(const std::string& uri, std::string& cookies) {
  DWORD buffer_size = 0;
  InternetGetCookie(uri.c_str(), nullptr, nullptr, &buffer_size);
  if (buffer_size == 0) return false;

  char* buffer = new char[buffer_size];
  InternetGetCookie(uri.c_str(), nullptr, buffer, &buffer_size);
  cookies.assign(buffer, buffer_size);
  return true;
}

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
