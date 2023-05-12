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

#include "vfs/handler/http_request.h"

namespace hippy {
inline namespace vfs {

constexpr char kHttpHeaders[] = "headers";
constexpr char kHttpMethod[] = "method";
constexpr char kHttpRedirect[] = "redirect";
constexpr char kHttpBody[] = "body";

HttpRequest::HttpRequest(string_view uri, std::unordered_map<std::string, std::string> meta)
    : uri_(std::move(uri)), meta_(std::move(meta)) {}

std::string HttpRequest::RequestMethod() const {
  for (const auto& [key, value] : meta_) {
    if (key == kHttpMethod && !value.empty()) return value;
  }
  return "GET";
}

bool HttpRequest::EnableFollowLocation() const {
  for (const auto& [key, value] : meta_) {
    if (key == kHttpRedirect && value == "follow") return true;
  }
  return false;
}

std::string HttpRequest::RequestHeaders() const { return meta_.count(kHttpHeaders) ? meta_.at(kHttpHeaders) : ""; }

std::string HttpRequest::RequestBody() const { return meta_.count(kHttpBody) ? meta_.at(kHttpBody) : ""; }

// TODO add user agent
std::string HttpRequest::RequestUserAgent() const { return ""; }

}  // namespace vfs
}  // namespace hippy
