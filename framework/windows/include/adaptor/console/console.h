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

#include <Windows.h>

#include "footstone/hippy_value.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

class Console {
 public:
  Console(std::string app_name, bool debug);
  ~Console() = default;

  bool Initial();
  void Info(const std::string& message);
  void Warn(const std::string& message);
  void Error(const std::string& message);
  void Fatal(const std::string& message);

 private:
  void WriteLog(const std::string message, WORD event_type);

 private:
  std::string app_name_;
  bool debug_;
};

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
