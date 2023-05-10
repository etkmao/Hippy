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

#include "adaptor/storage/sqlite_storage.h"

#include <memory>

#include "footstone/logging.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

Sqlite::~Sqlite() {
  if (db_) sqlite3_close(db_);
};

bool Sqlite::OpenDataBase() {
  std::lock_guard<std::mutex> lock_gard(mutex_);
  if (db_) {
    FOOTSTONE_LOG(WARNING) << "Sqlite db has been init !!!";
    return true;
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

  return true;
}

bool Sqlite::CreateTableIfNotExist() {
  if (db_ == nullptr) return false;
  char* error_message = nullptr;
  std::string sql_string =
      "CREATE TABLE IF NOT EXISTS " + table_name_ + " (key TEXT PRIMARY KEY, value TEXT NOT NULL);";
  int ret = sqlite3_exec(db_, sql_string.c_str(), NULL, 0, &error_message);
  if (ret != SQLITE_OK) {
    FOOTSTONE_LOG(WARNING) << "Sqlite create table if not exists error !!! Can't create table: " << sqlite3_errmsg(db_);
    sqlite3_free(error_message);
    return false;
  }
  return true;
}

bool Sqlite::Query(const std::string& query) {
  if (db_ == nullptr) return false;
  char* error_message = nullptr;
  int ret = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &error_message);
  if (ret != SQLITE_OK) {
    FOOTSTONE_LOG(WARNING) << "Sqlite query error !!! Query string [" << query
                           << "] !!! Can't query table: " << sqlite3_errmsg(db_);
    sqlite3_free(error_message);
    return false;
  }
  return true;
}

bool Sqlite::Query(const std::string& query, std::vector<std::string>& values) {
  if (db_ == nullptr) return false;
  char* error_message = nullptr;

  auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
    auto values = static_cast<std::vector<std::string>*>(data);
    values->push_back(argv[0]);
    return 0;
  };
  int ret = sqlite3_exec(db_, query.c_str(), callback, &values, &error_message);
  if (ret != SQLITE_OK) {
    FOOTSTONE_LOG(WARNING) << "Sqlite query error !!! Query string [" << query
                           << "] !!! Can't query table: " << sqlite3_errmsg(db_);
    sqlite3_free(error_message);
    return false;
  }
  return true;
}

bool Sqlite::Query(const std::string& query, std::unordered_map<std::string, std::string>& values) {
  if (db_ == nullptr) return false;
  char* error_message = nullptr;

  auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
    auto values = static_cast<std::unordered_map<std::string, std::string>*>(data);
    if (argc != 2) return -1;
    values->insert({argv[0], argv[1]});
    return 0;
  };
  int ret = sqlite3_exec(db_, query.c_str(), callback, &values, &error_message);
  if (ret != SQLITE_OK) {
    FOOTSTONE_LOG(WARNING) << "Sqlite query error !!! Query string [" << query
                           << "] !!! Can't query table: " << sqlite3_errmsg(db_);
    sqlite3_free(error_message);
    return false;
  }
  return true;
}

bool Sqlite::Transaction(const std::vector<std::string>& querys) {
  if (db_ == nullptr) return false;
  char* error_message = nullptr;

  int ret = sqlite3_exec(db_, "BEGIN TRANSACTION;", nullptr, nullptr, &error_message);
  if (ret != SQLITE_OK) {
    FOOTSTONE_LOG(WARNING) << "Sqlite transaction error !!! Can't begin transaction: " << sqlite3_errmsg(db_);
    sqlite3_free(error_message);
    return false;
  }

  for (const auto& query : querys) {
    ret = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &error_message);
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

std::string Sqlite::ConditionToSelect(const std::vector<std::string>& keys) {
  std::string query;
  if (keys.empty()) {
    return "select * from " + table_name_ + ";";
  }
  query = "select * from " + table_name_ + " where key in (";
  auto last_index = keys.size() - 1;
  for (auto i = 0; i < keys.size(); i++) {
    query += "'";
    query += keys[i];
    if (i != last_index) {
      query += "', ";
    } else {
      query += "'";
    }
  }
  query += ")";
  return query;
};

std::string Sqlite::Select() { return "select key from " + table_name_ + ";"; }

std::vector<std::string> Sqlite::ConditionToInsert(const std::unordered_map<std::string, std::string>& kvs) {
  std::vector<std::string> querys;
  for (const auto& kv : kvs) {
    std::string query;
    query = "insert into " + table_name_ + " (key, value) values ('" + kv.first + "', '" + kv.second + "');";
    querys.push_back(query);
  }
  return querys;
};

std::string Sqlite::ConditionToDelete(const std::vector<std::string>& keys) {
  std::string query;
  if (keys.empty()) return query;

  query = "delete from " + table_name_ + " where key in (";
  auto last_index = keys.size() - 1;
  for (auto i = 0; i < keys.size(); i++) {
    query += "'";
    query += keys[i];
    if (i != last_index) {
      query += "', ";
    } else {
      query += "'";
    }
  }
  query += ")";
  return query;
}

SqliteStorage::SqliteStorage() : sqlite_(std::make_unique<Sqlite>()){};

bool SqliteStorage::Initial() {
  if (sqlite_ == nullptr) return false;

  auto ret = sqlite_->OpenDataBase();
  if (!ret) return false;

  ret = sqlite_->CreateTableIfNotExist();
  if (!ret) return false;

  return true;
}

void SqliteStorage::GetItemsValue(std::vector<std::string> keys,
                                  std::function<void(const footstone::value::HippyValue&)> success_callback,
                                  std::function<void(const footstone::value::HippyValue&)> fail_callback) {
  std::unordered_map<std::string, std::string> kvs;
  std::string query;
  query = sqlite_->ConditionToSelect(keys);
  bool ret = sqlite_->Query(query, kvs);
  if (!ret) {
    fail_callback(footstone::HippyValue("Get items value failed!!"));
  } else {
    footstone::HippyValue params;
    if (kvs.size() == 0) {
      success_callback(footstone::HippyValue::Null());
    } else {
      footstone::HippyValue::HippyValueArrayType array;
      for (const auto& kv : kvs) {
        footstone::HippyValue::HippyValueArrayType kv_array;
        kv_array.push_back(footstone::HippyValue(kv.first));
        kv_array.push_back(footstone::HippyValue(kv.second));
        array.push_back(footstone::HippyValue(kv_array));
      }
      success_callback(footstone::HippyValue(array));
    }
  }
  return;
};

void SqliteStorage::SetItemsValue(std::unordered_map<std::string, std::string> kvs,
                                  std::function<void(const footstone::value::HippyValue&)> success_callback,
                                  std::function<void(const footstone::value::HippyValue&)> fail_callback) {
  std::vector<std::string> querys;
  querys = sqlite_->ConditionToInsert(kvs);
  bool ret = sqlite_->Transaction(querys);
  if (!ret) {
    fail_callback(footstone::HippyValue("Set items value failed!!"));
  } else {
    success_callback(footstone::HippyValue("sucess"));
  }
};

void SqliteStorage::RemoveItems(std::vector<std::string> keys,
                                std::function<void(const footstone::value::HippyValue&)> success_callback,
                                std::function<void(const footstone::value::HippyValue&)> fail_callback) {
  std::string query;
  query = sqlite_->ConditionToDelete(keys);
  if (query.empty()) {
    fail_callback(footstone::HippyValue("Delete Empty keys!!"));
  }
  bool ret = sqlite_->Query(query);
  if (!ret) {
    fail_callback(footstone::HippyValue("Delete items value failed!!"));
  } else {
    success_callback(footstone::HippyValue::Null());
  }
};

void SqliteStorage::GetAllItemsKey(std::function<void(const footstone::value::HippyValue&)> success_callback,
                                   std::function<void(const footstone::value::HippyValue&)> fail_callback) {
  std::vector<std::string> values;
  std::string query;
  query = sqlite_->Select();
  bool ret = sqlite_->Query(query, values);
  if (!ret) {
    fail_callback(footstone::HippyValue("Get items value failed!!"));
  } else {
    footstone::HippyValue::HippyValueArrayType array;
    for (const auto& value : values) {
      array.push_back(footstone::HippyValue(value));
    }
    success_callback(footstone::HippyValue(array));
  }
};

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
