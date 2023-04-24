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

#include <string>

#include "footstone/string_view.h"

namespace hippy {
inline namespace framework {
inline namespace windows {
inline namespace config {

constexpr uint32_t kInvalidInitialHeapSize = -1;
constexpr uint32_t kInvalidMaximumHeapSize = -1;
constexpr char kDefaultCodeCacheDirectory[] = "./";

// TODO 需要修改
constexpr char kDefaultGlobalConfig[] =
    "{\"Platform\":{\"OS\":\"android\",\"PackageName\":\"com.tencent.mtt.hippy.example\",\"APILevel\":28,"
    "\"VersionName\":\"2.3.4\",\"Localization\":{\"country\":\"CN\",\"language\":\"zh\",\"direction\":0},"
    "\"NightMode\":false},\"Dimensions\":{\"screenPhysicalPixels\":{\"densityDpi\":560.0,\"width\":411,\"scale\":3.5,"
    "\"statusBarHeight\":24.0,\"navigationBarHeight\":48.0,\"fontScale\":3.5,\"height\":846},"
    "\"windowPhysicalPixels\":{\"densityDpi\":560.0,\"width\":411,\"scale\":3.5,\"statusBarHeight\":24.0,"
    "\"navigationBarHeight\":48.0,\"fontScale\":3.8500001430511475,\"height\":798}},\"tkd\":{\"appVersion\":\"2.3."
    "4\",\"appName\":\"com.tencent.mtt.hippy.example\",\"extra\":{},\"url\":\"\"}}";

using string_view = footstone::stringview::string_view;

class Config {
 public:
  Config();
  Config(bool enabel_v8_serialization, bool dev_module, bool use_code_cache, int64_t group_id, uint32_t devtools_id,
         string_view code_cache_directory = kDefaultCodeCacheDirectory,
         string_view global_config = kDefaultGlobalConfig);
  ~Config() = default;

  void SetEnabelV8Serialization(bool enabel_v8_serialization) { enabel_v8_serialization_ = enabel_v8_serialization; }
  bool GetEnabelV8Serialization() { return enabel_v8_serialization_; }

  void SetDevModule(bool dev_module) { dev_module_ = dev_module; }
  bool GetDevModule() { return dev_module_; }

  void SetUseCodeCache(bool use_code_cache) { use_code_cache_ = use_code_cache; }
  bool GetUseCodeCache() { return use_code_cache_; }

  void SetGroupId(int64_t group_id) { group_id_ = group_id; }
  int64_t GetGroupId() { return group_id_; }

  void SetDevtoolsId(uint32_t devtools_id) { devtools_id_ = devtools_id; }
  uint32_t GetDevtoolsId() { return devtools_id_; }

  void SetGlobalConfig(string_view global_config) { global_config_ = global_config; }
  string_view GetGlobalConfig() { return global_config_; }

  void SetInitalHeapSize(uint32_t byte_size) { initial_heap_size_in_bytes_ = byte_size; }
  uint32_t GetInitalHeapSize() { return initial_heap_size_in_bytes_; }

  void SetMaximumHeapSize(uint32_t byte_size) { maximum_heap_size_in_bytes_ = byte_size; }
  uint32_t GetMaximumHeapSize() { return maximum_heap_size_in_bytes_; }

  void SetCodeCacheDirectory(string_view code_cache_directory) { code_cache_directory_ = code_cache_directory; }
  string_view GetCodeCacheDirectory() { return code_cache_directory_; }

  std::string LoadInstanceString(uint32_t root_id);

 private:
  bool enabel_v8_serialization_;
  bool dev_module_;
  bool use_code_cache_;
  int64_t group_id_{10};
  uint32_t devtools_id_{10};
  string_view code_cache_directory_;
  string_view global_config_;
  uint32_t initial_heap_size_in_bytes_{kInvalidInitialHeapSize};
  uint32_t maximum_heap_size_in_bytes_{kInvalidMaximumHeapSize};
};

}  // namespace config
}  // namespace windows
}  // namespace framework
}  // namespace hippy
