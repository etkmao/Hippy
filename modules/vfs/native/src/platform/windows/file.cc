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

#include "vfs/file.h"

#include <filesystem>
#include <iostream>

namespace hippy {
inline namespace vfs {

using StringViewUtils = footstone::stringview::StringViewUtils;

bool HippyFile::SaveFile(const string_view& file_path, const std::string& content, std::ios::openmode mode) {
  FOOTSTONE_DLOG(INFO) << "SaveFile file_path = " << file_path;
  auto path_str = StringViewUtils::ConvertEncoding(file_path, string_view::Encoding::Utf8).utf8_value();
  std::ofstream file(reinterpret_cast<const char*>(path_str.c_str()), mode);
  if (file.is_open()) {
    std::streamsize len;
    bool is_success = footstone::numeric_cast<size_t, std::streamsize>(content.length(), len);
    if (is_success) {
      file.write(content.c_str(), len);
    }
    file.close();
    return is_success;
  } else {
    return false;
  }
}

int HippyFile::RmFullPath(const string_view& dir_full_path) {
  FOOTSTONE_DLOG(INFO) << "RmFullPath dir_full_path = " << dir_full_path;
  auto str = StringViewUtils::ConvertEncoding(dir_full_path, string_view::Encoding::Utf8).utf8_value();
  std::filesystem::path path(reinterpret_cast<const char*>(str.c_str()));
  std::filesystem::remove_all(path);
  FOOTSTONE_DLOG(INFO) << "RmFullPath succ";
  return 0;
}

int HippyFile::CreateDir(const string_view& dir_path) {
  FOOTSTONE_DLOG(INFO) << "CreateDir path = " << dir_path;
  auto str = StringViewUtils::ConvertEncoding(dir_path, string_view::Encoding::Utf8).utf8_value();
  std::filesystem::path path(reinterpret_cast<const char*>(str.c_str()));
  if (std::filesystem::create_directory(path)) return 0;
  return -1;
}

int HippyFile::CheckDir(const string_view& dir_path) {
  FOOTSTONE_DLOG(INFO) << "CheckDir path = " << dir_path;
  auto str = StringViewUtils::ConvertEncoding(dir_path, string_view::Encoding::Utf8).utf8_value();
  std::filesystem::path path(reinterpret_cast<const char*>(str.c_str()));
  if (std::filesystem::is_directory(path)) return 0;
  return -1;
}

uint64_t HippyFile::GetFileModifyTime(const string_view& file_path) {
  FOOTSTONE_DLOG(INFO) << "GetFileModifyTime file_path = " << file_path;
  auto path_str = StringViewUtils::ConvertEncoding(file_path, string_view::Encoding::Utf8).utf8_value();
  std::filesystem::path path(reinterpret_cast<const char*>(path_str.c_str()));
  std::filesystem::file_time_type ftime = std::filesystem::last_write_time(path);
  FOOTSTONE_DLOG(INFO) << "modify_time = " << ftime.time_since_epoch().count();
  return static_cast<uint64_t>(ftime.time_since_epoch().count());
}

}  // namespace vfs
}  // namespace hippy
