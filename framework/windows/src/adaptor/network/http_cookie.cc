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

#include <cstdlib>
#include <regex>
#include <sstream>
#include <vector>

#include "footstone/logging.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

HttpCookie::~HttpCookie() {
  if (db_) sqlite3_close(db_);
};

bool HttpCookie::Initial() {
  std::lock_guard<std::mutex> lock_gard(mutex_);
  if (db_) {
    FOOTSTONE_LOG(WARNING) << "Sqlite db has been init !!!";
    return false;
  }

  if (db_name_.empty()) {
    FOOTSTONE_LOG(WARNING) << "Sqlite db name is empty !!!";
    return false;
  }

  int ret = sqlite3_open(db_name_.c_str(), &db_);
  if (ret != SQLITE_OK) {
    FOOTSTONE_LOG(WARNING) << "Sqlite open error !!! Can't open database: " << sqlite3_errmsg(db_);
    return false;
  }

  char* error_message = nullptr;
  std::string create_table =
      "CREATE TABLE IF NOT EXISTS " + table_name_ +
      " (domain TEXT Not NULL, path TEXT, cookie TEXT NOT NULL, PRIMARY KEY (domain, path, cookie));";
  ret = sqlite3_exec(db_, create_table.c_str(), NULL, 0, &error_message);
  if (ret != SQLITE_OK) {
    FOOTSTONE_LOG(WARNING) << "Sqlite create table if not exists error !!! Can't create table: " << sqlite3_errmsg(db_);
    sqlite3_free(error_message);
    return false;
  }
  return true;
}

static std::vector<std::string> Split(const std::string& split_string, char delim) {
  std::vector<std::string> vec;
  std::stringstream ss(split_string);
  std::string temp;
  while (std::getline(ss, temp, delim)) {
    vec.push_back(temp);
  }
  return vec;
}

static std::string NormalizeCookie(const std::string& cookie, const std::string& expires) {
  auto kv = Split(cookie, '=');
  if (kv.size() == 1 || (kv.size() >= 2 && kv[1].empty())) {
    return kv[0] + "=;Max-Age=0";
  }
  if (!expires.empty()) {
    return cookie + ";expires=" + expires;
  }
  return cookie;
}

bool HttpCookie::SetCookie(const std::string& url, const std::string& cookies, const std::string& expires) {
  if (db_ == nullptr) return false;
  if (!url.empty() && !cookies.empty()) {
    // ClearCookies(url);
    if (cookies.empty()) {
      return ClearCookies(url);
    } else {
      return InsertCookies(url, cookies, expires);
    }
  }
  return false;
}

bool HttpCookie::GetCookie(const std::string& url, std::function<void(footstone::value::HippyValue)> callback) {
  std::string domain, path;
  if (ParsedUrl(url, domain, path)) {
    std::string query;
    if (path.empty()) {
      query = "select cookie from " + table_name_ + " where domain = '" + domain + "'";
    } else {
      query = "select cookie from " + table_name_ + " where domain = '" + domain + "' and path = '" + path + "';";
    }

    char* error_message = nullptr;
    std::vector<std::string> cookie_vector;
    auto sql_callback = [](void* data, int argc, char** argv, char** colName) -> int {
      auto cookie_vector = static_cast<std::vector<std::string>*>(data);
      cookie_vector->push_back((*argv));
      return 0;
    };
    auto ret = sqlite3_exec(db_, query.c_str(), sql_callback, &cookie_vector, &error_message);
    if (ret != SQLITE_OK) {
      FOOTSTONE_LOG(WARNING) << "Sqlite query error !!! Query string [" << query
                             << "] !!! Can't query table: " << sqlite3_errmsg(db_);
      sqlite3_free(error_message);
      return false;
    }
    std::string call_back_string;
    for (size_t i = 0; i < cookie_vector.size(); i++) {
      auto s = Split(cookie_vector[i], ';');
      call_back_string += s[0];
      if (i != cookie_vector.size() - 1) call_back_string += ";";
    }
    callback(footstone::value::HippyValue(call_back_string));
    return true;
  }
  return false;
}

bool HttpCookie::GetCookies(const std::string& url, std::vector<std::string>& cookies) {
  std::string domain, path;
  if (ParsedUrl(url, domain, path)) {
    std::string query;
    if (path.empty()) {
      query = "select cookie from " + table_name_ + " where domain = '" + domain + "'";
    } else {
      query = "select cookie from " + table_name_ + " where domain = '" + domain + "' and path = '" + path + "';";
    }

    char* error_message = nullptr;
    auto sql_callback = [](void* data, int argc, char** argv, char** colName) -> int {
      auto cookies = static_cast<std::vector<std::string>*>(data);
      cookies->push_back((*argv));
      return 0;
    };
    auto ret = sqlite3_exec(db_, query.c_str(), sql_callback, &cookies, &error_message);
    if (ret != SQLITE_OK) {
      FOOTSTONE_LOG(WARNING) << "Sqlite query error !!! Query string [" << query
                             << "] !!! Can't query table: " << sqlite3_errmsg(db_);
      sqlite3_free(error_message);
      return false;
    }
    return true;
  }
  return false;
}

bool HttpCookie::ParsedUrl(const std::string& url, std::string& domain, std::string& path) {
  std::regex pattern("(http|https)://([^/ :]+):?([^/ ]*)(/?[^ #?]*)\\x3f?([^ #]*)#?([^ ]*)");
  std::smatch match;
  if (std::regex_search(url, match, pattern)) {
    FOOTSTONE_DLOG(INFO) << "protocol = " << match[1].str() << ", domain = " << match[2].str()
                         << ", port = " << match[3].str() << ", path = " << match[4].str()
                         << ", query = " << match[5].str() << ", fragment = " << match[6].str();
    domain = match[2].str();
    path = match[4].str();
    return true;
  }
  return false;
}

bool HttpCookie::ClearCookies(const std::string& url) {
  std::string domain, path;
  if (ParsedUrl(url, domain, path)) {
    if (domain.empty()) return false;

    // 查询对应 domain 和 path 的 cookie 数据
    std::vector<std::string> cookie_vector;
    std::string query = "select cookie from " + table_name_;
    if (path.empty()) {
      query += " where domain = '" + domain + "';";
    } else {
      query += " where domain = '" + domain + " and path = '" + path + "';";
    }
    char* error_message = nullptr;
    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
      auto cookie_vector = static_cast<std::vector<std::string>*>(data);
      cookie_vector->push_back((*argv));
      return 0;
    };
    auto ret = sqlite3_exec(db_, query.c_str(), callback, &cookie_vector, &error_message);
    if (ret != SQLITE_OK) {
      FOOTSTONE_LOG(WARNING) << "Sqlite query error !!! Query string [" << query
                             << "] !!! Can't query table: " << sqlite3_errmsg(db_);
      sqlite3_free(error_message);
      return false;
    }

    // 更新 cookie expire 为 Max-Age=0
    std::vector<std::string> update_vector;
    for (const auto& cookie : cookie_vector) {
      auto splits = Split(cookie, ';');
      FOOTSTONE_DCHECK(splits.size() == 1 || splits.size() == 2);
      if (splits.size() != 1 && splits.size() != 2) return false;
      if (splits.size() == 1 || (splits.size() == 2 && splits[1] != ";Max-Age=0")) {
        update_vector.push_back(splits[0] + ";Max-Age=0");
      }
    }
    if (update_vector.size() == 0) return true;

    ret = sqlite3_exec(db_, "BEGIN TRANSACTION;", nullptr, nullptr, &error_message);
    if (ret != SQLITE_OK) {
      FOOTSTONE_LOG(WARNING) << "Sqlite transaction error !!! Can't begin transaction: " << sqlite3_errmsg(db_);
      sqlite3_free(error_message);
      return false;
    }
    std::string update_query;
    for (const auto& cookie : update_vector) {
      if (path.empty()) {
        update_query += "update " + table_name_ + " set cookie = '" + cookie + "' where domain = '" + domain + "';";
      } else {
        update_query += "update " + table_name_ + " set cookie = '" + cookie + "' where domain = '" + domain +
                        " and path = '" + path + "';";
      }
      ret = sqlite3_exec(db_, update_query.c_str(), nullptr, nullptr, &error_message);
      if (ret != SQLITE_OK) {
        FOOTSTONE_LOG(WARNING) << "Sqlite transaction error !!! Query string [" << query
                               << "] !!! Can't query table: " << sqlite3_errmsg(db_);
        sqlite3_free(error_message);
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
      }
    }
    ret = sqlite3_exec(db_, "COMMIT;", nullptr, nullptr, &error_message);
    if (ret != SQLITE_OK) {
      FOOTSTONE_LOG(WARNING) << "Sqlite transaction error !!! Can't commit transaction: " << sqlite3_errmsg(db_);
      sqlite3_free(error_message);
      sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
      return false;
    }
    return true;
  }
}

std::string HttpCookie::DeleteCookieSql(const std::string& domain, const std::string& path) {
  std::string sql;
  //   if (path.empty()) {
  //     sql += "update " + table_name_ + " set cookie = cookie||;Max-Age=0" +  + " where domain = '" + domain + "' and
  //     ;";
  //   } else {
  //     sql += "update " + table_name_ + " where domain = '" + domain + "' and path = '" + path + "';";
  //   }
  return sql;
}

bool HttpCookie::InsertCookies(const std::string& url, const std::string& cookies, const std::string& expires) {
  std::regex reg("\\s+");
  std::string cookies_format = regex_replace(cookies, reg, "");
  std::vector<std::string> cookie_vector, normalized_cookie_vector;
  cookie_vector = Split(cookies_format, ';');
  for (auto& s : cookie_vector) {
    std::string cookie = NormalizeCookie(s, expires);
    normalized_cookie_vector.push_back(cookie);
  }

  std::string domain, path;
  if (ParsedUrl(url, domain, path)) {
    if (domain.empty()) return false;

    char* error_message = nullptr;
    int ret = sqlite3_exec(db_, "BEGIN TRANSACTION;", nullptr, nullptr, &error_message);
    if (ret != SQLITE_OK) {
      FOOTSTONE_LOG(WARNING) << "Sqlite transaction error !!! Can't begin transaction: " << sqlite3_errmsg(db_);
      sqlite3_free(error_message);
      return false;
    }

    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
      auto update_vector = static_cast<std::vector<CookieUpdateType>*>(data);
      if (std::atoi(*argv) == 1) {
        update_vector->push_back(CookieUpdateType::kUpdate);
      } else {
        update_vector->push_back(CookieUpdateType::kInsert);
      }
      return 0;
    };
    std::vector<CookieUpdateType> update_vector;
    std::vector<std::string> querys = QueryCookiesSql(domain, path, normalized_cookie_vector);
    for (const auto& query : querys) {
      ret = sqlite3_exec(db_, query.c_str(), callback, &update_vector, &error_message);
      if (ret != SQLITE_OK) {
        FOOTSTONE_LOG(WARNING) << "Sqlite transaction error !!! Query string [" << query
                               << "] !!! Can't query table: " << sqlite3_errmsg(db_);
        sqlite3_free(error_message);
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
      }
    }

    if (normalized_cookie_vector.size() != update_vector.size()) return false;

    std::vector<std::string> sqls = InsertOrUpdateCookiesSql(domain, path, normalized_cookie_vector, update_vector);
    for (const auto& sql : sqls) {
      ret = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &error_message);
      if (ret != SQLITE_OK) {
        FOOTSTONE_LOG(WARNING) << "Sqlite transaction error !!! Query string [" << sql
                               << "] !!! Can't query table: " << sqlite3_errmsg(db_);
        sqlite3_free(error_message);
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
      }
    }

    ret = sqlite3_exec(db_, "COMMIT;", nullptr, nullptr, &error_message);
    if (ret != SQLITE_OK) {
      FOOTSTONE_LOG(WARNING) << "Sqlite transaction error !!! Can't commit transaction: " << sqlite3_errmsg(db_);
      sqlite3_free(error_message);
      sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
      return false;
    }
  }
  return true;
}

std::vector<std::string> HttpCookie::InsertOrUpdateCookiesSql(
    const std::string& domain, const std::string& path, const std::vector<std::string>& cookies,
    const std::vector<CookieUpdateType>& need_insert_or_update) {
  std::vector<std::string> sqls;
  for (size_t i = 0; i < cookies.size(); i++) {
    if (need_insert_or_update[i] == CookieUpdateType::kInsert) {
      auto cookie = cookies[i];
      if (path.empty()) {
        sqls.push_back(
            {"insert into " + table_name_ + " (domain, cookie) values ('" + domain + "', '" + cookie + "');"});
      } else {
        sqls.push_back({"insert into " + table_name_ + " (domain, path, cookie) values ('" + domain + "', '" + path +
                        "', '" + cookie + "');"});
      }
    } else if (need_insert_or_update[i] == CookieUpdateType::kUpdate) {
      auto cookie = cookies[i];
      auto s = Split(cookie, ';');
      if (path.empty()) {
        sqls.push_back({"update " + table_name_ + " set cookie = '" + cookie + "' where domain = '" + domain +
                        "' and substr(cookie, 0, " + std::to_string(s[0].size() + 1) + ")  = '" + s[0] + "';"});
      } else {
        sqls.push_back({"update " + table_name_ + " set cookie = '" + cookie + "' where domain = '" + domain +
                        " and path = '" + path + "' and substr(cookie, 0, " + std::to_string(s[0].size() + 1) +
                        ")  = '" + s[0] + "';"});
      }
    }
  }
  return sqls;
}

std::vector<std::string> HttpCookie::QueryCookiesSql(const std::string& domain, const std::string& path,
                                                     const std::vector<std::string>& cookies) {
  std::vector<std::string> sqls;
  for (const auto& cookie : cookies) {
    auto s = Split(cookie, ';');
    if (path.empty()) {
      sqls.push_back({"select count(*), cookie from " + table_name_ + " where domain = '" + domain +
                      "' and substr(cookie, 0, " + std::to_string(s[0].size() + 1) + ")  = '" + s[0] + "';"});
    } else {
      sqls.push_back({"select count(*), cookie from " + table_name_ + " where domian = '" + domain + "' and path = '" +
                      path + "' and substr(cookie, 0, " + std::to_string(s[0].size() + 1) + ")  = '" + s[0] + "';"});
    }
  }
  return sqls;
}

// std::vector<std::string> HttpCookie::QueryCookiesSql(const std::string& domain, const std::string& path) {
//   std::vector<std::string> sqls;
// //   for (const auto& cookie : cookies) {
//     if (path.empty()) {
//       sqls.push_back(
//           {"select count(*) from " + table_name_ + " where domain = '" + domain + "' and cookie = '" + cookie +
//           "';"});
//     } else {
//       sqls.push_back({"select count(*) from " + table_name_ + " where domian = '" + domain + "' and path = '" + path
//       +
//                       "' and cookie = '" + cookie + "';"});
//     }
// //   }
//   return sqls;
// }

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
