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
#include <vector>

#include "footstone/hippy_value.h"

namespace hippy {
inline namespace vfs {

class HttpResponse {
 public:
  HttpResponse(std::vector<uint8_t> header, std::vector<uint8_t> body);
  ~HttpResponse() = default;

  void Parser();
  std::string GetHttpVersion() { return http_version_; }
  std::string GetStatusCode() { return status_code_; }
  std::string GetStatusMessage() { return status_message_; }
  void ResponseBuffer(std::string& buffer);

 private:
  std::vector<uint8_t> header_;
  std::vector<uint8_t> body_;
  std::string http_version_;
  std::string status_code_;
  std::string status_message_;
  std::unordered_map<std::string, std::string> header_fields_;
};

}  // namespace vfs
}  // namespace hippy
