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

#include "vfs/handler/http_response.h"

#include <sstream>

#include "footstone/serializer.h"

namespace hippy {
inline namespace vfs {

constexpr char kStatusCode[] = "statusCode";
constexpr char kStatusLine[] = "statusLine";
constexpr char kResponseHeaders[] = "respHeaders";
constexpr char kResponseBody[] = "respBody";

HttpResponse::HttpResponse(std::vector<uint8_t> header, std::vector<uint8_t> body)
    : header_(std::move(header)), body_(std::move(body)) {}

static std::vector<std::string> Split(const std::string& str, const std::string& delimiter) {
  std::vector<std::string> tokens;
  size_t start = 0, end = 0;
  while ((end = str.find(delimiter, start)) != std::string::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + delimiter.length();
  }
  tokens.push_back(str.substr(start));
  return tokens;
}

void HttpResponse::Parser() {
  if (header_.size() > 0) {
    std::string header_string(reinterpret_cast<const char*>(&header_[0]), header_.size());
    std::vector<std::string> headers;
    size_t pos = 0;
    std::string line;
    while ((pos = header_string.find("\r\n")) != std::string::npos) {
      line = header_string.substr(0, pos);
      header_string.erase(0, pos + 2);
      if (!line.empty()) headers.push_back(line);
    }

    std::vector<std::string> parts = Split(headers[0], " ");
    if (parts.size() == 3) {
      http_version_ = parts[0];
      status_code_ = parts[1];
      status_message_ = parts[2];
    }

    if (headers.size() > 1) {
      for (size_t i = 1; i < headers.size(); i++) {
        std::vector<std::string> header_parts = Split(headers[i], ":");
        header_fields_.insert({header_parts[0], header_parts[1]});
      }
    }
  }
}

void HttpResponse::ResponseBuffer(std::string& buffer) {
  footstone::HippyValue::HippyValueObjectType object;
  object.insert({kStatusCode, footstone::HippyValue(status_code_)});
  object.insert({kStatusLine, footstone::HippyValue(status_message_)});
  footstone::HippyValue::HippyValueObjectType header_fields_object;
  for (const auto& [key, value] : header_fields_) {
    header_fields_object.insert({key, footstone::HippyValue(value)});
  }
  object.insert({kResponseHeaders, footstone::HippyValue(header_fields_object)});
  std::string body(body_.begin(), body_.end());
  object.insert({kResponseBody, footstone::HippyValue(body)});

  footstone::value::Serializer serializer;
  serializer.WriteHeader();
  serializer.WriteValue(footstone::HippyValue(object));
  std::pair<uint8_t*, size_t> serializer_buffer = serializer.Release();
  buffer = std::string(reinterpret_cast<const char*>(serializer_buffer.first), serializer_buffer.second);
  return;
}

}  // namespace vfs
}  // namespace hippy
