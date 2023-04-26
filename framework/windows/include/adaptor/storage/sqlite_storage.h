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
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "adaptor/storage/storage.h"
#include "sqlite/sqlite3.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

using StorageResponse = hippy::windows::framework::module::StorageResponse;

class Sqlite {
 public:
  Sqlite() = default;
  ~Sqlite();

  std::string GetDatabaseName() { return db_name_; }
  void SetDatabaseName(std::string db_name) { db_name_ = db_name; }
  std::string GetTableName() { return table_name_; }
  void SetTableName(std::string table_name) { table_name_ = table_name; }

  bool OpenDataBase();
  bool CreateTableIfNotExist();
  bool Query(const std::string& query);
  bool Query(const std::string& query, std::vector<std::string>& values);
  bool Query(const std::string& query, std::unordered_map<std::string, std::string>& values);
  bool Transaction(const std::vector<std::string>& querys);

  std::string Select();
  std::string ConditionToSelect(const std::vector<std::string>& keys);
  std::vector<std::string> ConditionToInsert(const std::unordered_map<std::string, std::string>& kvs);
  std::string ConditionToDelete(const std::vector<std::string>& keys);

 private:
  std::mutex mutex_;
  std::string db_name_{"db_hippy_storage"};
  std::string table_name_{"t_hippy_engine_storage"};
  sqlite3* db_;
};

class SqliteStorage : public Storage {
 public:
  SqliteStorage();
  virtual ~SqliteStorage() override = default;

  SqliteStorage(const SqliteStorage&) = delete;
  SqliteStorage(SqliteStorage&&) = delete;
  SqliteStorage& operator=(const SqliteStorage&) = delete;
  SqliteStorage& operator=(SqliteStorage&&) = delete;

  virtual void GetItemsValue(
      std::vector<std::string> keys,
      std::function<void(StorageResponse, std::unordered_map<std::string, std::string>)> callback) override;
  virtual void SetItemsValue(std::unordered_map<std::string, std::string> kvs,
                             std::function<void(StorageResponse)> callback) override;
  virtual void RemoveItems(std::vector<std::string> keys, std::function<void(StorageResponse)> callback) override;
  virtual void GetAllItemsKey(std::function<void(StorageResponse, std::vector<std::string>)> callback) override;

  virtual bool Initial() override;

 private:
  std::unique_ptr<Sqlite> sqlite_;
};

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
