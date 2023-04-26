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
#include <mutex>
#include <string>
#include <vector>

#include "footstone/hippy_value.h"
#include "sqlite/sqlite3.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

enum class CookieUpdateType {
  kNone,
  kInsert,
  kUpdate,
};

// TODO(charleeshen): COOKIE 估计需要重构
class HttpCookie {
 public:
  HttpCookie() = default;
  ~HttpCookie();

  bool Initial();

  bool SetCookie(const std::string& url, const std::string& cookies, const std::string& expires);
  bool GetCookie(const std::string& url, std::function<void(footstone::value::HippyValue)> callback);
  bool GetCookies(const std::string& url, std::vector<std::string>& cookies);

 private:
  bool ParsedUrl(const std::string& url, std::string& domain, std::string& path);
  bool ClearCookies(const std::string& url);
  std::string DeleteCookieSql(const std::string& domain, const std::string& path);
  bool InsertCookies(const std::string& url, const std::string& cookies, const std::string& expires);
  std::vector<std::string> InsertOrUpdateCookiesSql(const std::string& domain, const std::string& path,
                                                    const std::vector<std::string>& cookies,
                                                    const std::vector<CookieUpdateType>& need_insert_or_update);
  std::vector<std::string> QueryCookiesSql(const std::string& domain, const std::string& path,
                                           const std::vector<std::string>& cookies);

 private:
  std::mutex mutex_;
  sqlite3* db_;
  std::string db_name_{"db_hippy_cookie"};
  std::string table_name_{"t_hippy_cookie"};
};

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
