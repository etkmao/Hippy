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
#include <string>

#include "core/platform/common/tdf_engine.h"
#include "footstone/string_view.h"

namespace hippy {
inline namespace windows {
inline namespace framework {

constexpr uint32_t kInvalidInitialHeapSize = -1;
constexpr uint32_t kInvalidMaximumHeapSize = -1;
constexpr char kDefaultCodeCacheDirectory[] = "./";

using string_view = footstone::stringview::string_view;

class Config {
 public:
  Config();
  ~Config() = default;

 public:
  // 资源加载路径
  class JsAssetsPath {
   public:
    void SetCorePath(const std::string& core_path) { core_path_ = core_path; }
    std::string GetCorePath() { return core_path_; }
    void SetPreloadPath(const std::string& preload_path) { preload_path_ = preload_path; }
    std::string GetPreloadPath() { return preload_path_; }

   private:
    std::string core_path_;
    std::string preload_path_;
  };

  // 调试相关参数
  class Debug {
   public:
    void SetBundleName(const std::string& bundle_name) { bundle_name_ = bundle_name; }
    std::string GetBundleName() { return bundle_name_; }
    void SetHost(const std::string& host) { host_ = host; }
    std::string GetHost() { return host_; }
    void SetUrl(const std::string& url) { url_ = url; }
    std::string GetUrl() { return url_; }
    void SetDevelopmentModule(const bool is_development_module) { is_development_module_ = is_development_module; }
    bool GetDevelopmentModule() { return is_development_module_; }

   private:
    std::string bundle_name_;
    std::string host_;
    std::string url_;
    bool is_development_module_;
  };

  // 引擎相关参数
  class JsEngine {
   public:
    void SetEnableV8Serialization(bool enable_v8_serialization) { enable_v8_serialization_ = enable_v8_serialization; }
    bool GetEnableV8Serialization() { return enable_v8_serialization_; }
    void SetUseCodeCache(bool use_code_cache) { use_code_cache_ = use_code_cache; }
    bool GetUseCodeCache() { return use_code_cache_; }
    void SetGroupId(int64_t group_id) { group_id_ = group_id; }
    int64_t GetGroupId() { return group_id_; }
    void SetInitalHeapSize(uint32_t byte_size) { initial_heap_size_ = byte_size; }
    uint32_t GetInitalHeapSize() { return initial_heap_size_; }
    void SetMaximumHeapSize(uint32_t byte_size) { maximum_heap_size_ = byte_size; }
    uint32_t GetMaximumHeapSize() { return maximum_heap_size_; }
    void SetCodeCacheDirectory(string_view code_cache_directory) { code_cache_directory_ = code_cache_directory; }
    string_view GetCodeCacheDirectory() { return code_cache_directory_; }

   private:
    bool enable_v8_serialization_{true};
    bool use_code_cache_{true};
    int64_t group_id_{-1};
    uint32_t initial_heap_size_{kInvalidInitialHeapSize};
    uint32_t maximum_heap_size_{kInvalidMaximumHeapSize};
    string_view code_cache_directory_{kDefaultCodeCacheDirectory};
  };

  // TODO(charleeshen): 增加 Adaptor
  // class Adaptor {};

  void SetDensity(float density) { density_ = density; }
  float GetDensity() { return density_; }
  void SetRootId(uint32_t root_id) { root_id_ = root_id; }
  uint32_t GetRootId() { return root_id_; }

  // tdf shell
  void SetShell(std::shared_ptr<tdfcore::Shell> shell) { shell_ = shell; }
  std::shared_ptr<tdfcore::Shell> GetShell() { return shell_; }

  std::shared_ptr<JsAssetsPath> GetJsAssetsPath() const { return js_assets_path_; }
  std::shared_ptr<Debug> GetDebug() const { return debug_; }
  std::shared_ptr<JsEngine> GetJsEngine() const { return js_engine_; }

 private:
  float density_{1.0f};
  uint32_t root_id_{0};
  std::shared_ptr<JsAssetsPath> js_assets_path_{nullptr};
  std::shared_ptr<JsEngine> js_engine_{nullptr};
  std::shared_ptr<Debug> debug_{nullptr};
  std::shared_ptr<tdfcore::Shell> shell_{nullptr};
};

}  // namespace framework
}  // namespace windows
}  // namespace hippy
