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

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

enum class RetCode { Success, Failed };

class StorageResponse {
 public:
  StorageResponse(RetCode recode);
  StorageResponse(RetCode recode, std::string error_message);

  RetCode GetRetCode() { return code_; }
  void SetRetCode(RetCode code) { code_ = code; }

  std::string GetErrorMsg() { return msg_; }
  void SetErrorMsg(std::string msg) { msg_ = msg; }

 private:
  RetCode code_;
  std::string msg_;
};

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
