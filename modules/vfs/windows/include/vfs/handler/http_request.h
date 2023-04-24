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

#pragma once

#include <string>
#include <unordered_map>

#include "footstone/string_view.h"

namespace hippy {
inline namespace vfs {

constexpr uint32_t kDefaultTimeoutMS = 3000;

class HttpRequest {
 public:
  using string_view = footstone::string_view;
  HttpRequest(string_view url, std::unordered_map<std::string, std::string> headers,
              std::unordered_map<std::string, std::string> parameters);
  ~HttpRequest() = default;
  HttpRequest(const HttpRequest&) = default;
  HttpRequest(HttpRequest&&) = default;
  HttpRequest& operator=(const HttpRequest&) = default;
  HttpRequest& operator=(HttpRequest&&) = default;


  string_view GetUrl() const { return url_; }
  void SetUrl(string_view url) { url_ = url; }
  uint32_t GetConnectionTimeout() const { return connection_time_ms_; }
  void SetConnectionTimeout(uint32_t connection_time_ms) { connection_time_ms_ = connection_time_ms; }
  uint32_t GetReadTimeout() const { return read_time_ms_; }
  void SetReadTimeout(uint32_t read_time_ms) { read_time_ms_ = read_time_ms; }
  bool GetUseCache() const { return use_cache_; }
  void SetUseCache(bool use_cache) { use_cache_ = use_cache; }
  std::string GetCookie() const;

  std::string RequestMethod() const;
  bool EnableFollowLocation() const;
  std::unordered_map<std::string, std::string> RequestHeaders() const { return headers_; }
  std::string RequestBody() const;
  std::string RequestUserAgent() const;

 private:
  string_view url_;
  uint32_t connection_time_ms_{kDefaultTimeoutMS};
  uint32_t read_time_ms_{kDefaultTimeoutMS};
  bool use_cache_{true};

  std::unordered_map<std::string, std::string> headers_;
  std::unordered_map<std::string, std::string> parameters_;
};

}  // namespace vfs
}  // namespace hippy
