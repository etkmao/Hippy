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

#include "adaptor/network/http_network.h"

#include <memory>
#include <string>
#include <unordered_map>

#include "footstone/logging.h"
#include "footstone/serializer.h"
#include "vfs/handler/http_handler.h"
#include "vfs/handler/uri_handler.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

using string_view = footstone::stringview::string_view;

constexpr char kHttpHeaders[] = "headers";
constexpr char kResponseBody[] = "respBody";

HttpNetwork::HttpNetwork() : http_cookie_(std::make_shared<HttpCookie>()) {}

bool HttpNetwork::Initial() { return true; }

static void CallbackContent(const std::unordered_map<std::string, std::string>& meta, const std::string body,
                            std::string& callback_content) {
  footstone::HippyValue::HippyValueObjectType object;
  for (const auto& [key, value] : meta) {
    object.insert({key, footstone::HippyValue(value)});
  }
  object.insert({kResponseBody, footstone::HippyValue(body)});

  footstone::value::Serializer serializer;
  serializer.WriteHeader();
  serializer.WriteValue(footstone::HippyValue(object));
  std::pair<uint8_t*, size_t> serializer_buffer = serializer.Release();
  callback_content = std::string(reinterpret_cast<const char*>(serializer_buffer.first), serializer_buffer.second);
  return;
}

void HttpNetwork::Fetch(const std::shared_ptr<UriLoader>& uri_loader, const std::string& uri,
                        const footstone::value::HippyValue& request, std::function<void(std::string)> callback) {
  std::unordered_map<std::string, std::string> headers, parameters;
  auto ret = ParserRequestHeadersAndParameters(request, headers, parameters);
  FOOTSTONE_DCHECK(ret);
  if (ret) {
    std::unordered_map<std::string, std::string> meta;
    std::string headers_string;
    for (const auto& [key, value] : headers) {
      headers_string += key;
      headers_string += ":";
      headers_string += value;
      headers_string += "\r\n";
    }
    if (!headers_string.empty()) meta.insert({kHttpHeaders, headers_string});

    for (const auto& parameter : parameters) {
      meta.insert(parameter);
    }

    auto new_callback = [callback](hippy::JobResponse::RetCode rsp_retcode,
                                   std::unordered_map<std::string, std::string> rsp_meta,
                                   const JobResponse::bytes rsp_content) {
      if (rsp_retcode == hippy::JobResponse::RetCode::Success) {
        std::string callback_content;
        CallbackContent(rsp_meta, rsp_content, callback_content);
        callback(callback_content);
        return;
      }
    };
    uri_loader->RequestUntrustedContent(string_view(uri.c_str()), meta, new_callback);
  }
}

void HttpNetwork::GetCookie(const std::string& url, std::function<void(footstone::value::HippyValue)> callback) {
  std::string cookies;
  http_cookie_->GetCookies(url, cookies);
  callback(footstone::value::HippyValue(cookies));
  return;
}

void HttpNetwork::SetCookie(const footstone::value::HippyValue& request) {
  std::string url, kv, expires;
  if (ParserRequestCookie(request, url, kv, expires)) {
    http_cookie_->SetCookies(url, kv, expires);
  }
  return;
}

bool HttpNetwork::ParserRequestCookie(const footstone::value::HippyValue& request, std::string& url, std::string& kv,
                                      std::string& expires) {
  footstone::value::HippyValue::HippyValueArrayType array;
  if (!request.ToArray(array) || array.size() != 3) return false;
  if (!array[0].ToString(url) || !array[1].ToString(kv) || !array[2].ToString(expires)) return false;
  return true;
}

void HttpNetwork::ParserRequestHeaders(const footstone::value::HippyValue& headers,
                                       std::unordered_map<std::string, std::string>& parsed_headers) {
  footstone::value::HippyValue::HippyValueObjectType object_headers;
  if (!headers.ToObject(object_headers)) return;
  for (const auto& kv : object_headers) {
    if (kv.second.IsString()) {
      parsed_headers.insert({kv.first, kv.second.ToStringChecked()});
    } else if (kv.second.IsArray()) {
      footstone::value::HippyValue::HippyValueArrayType array;
      kv.second.ToArray(array);
      if (array.size() == 0) {
        return;
      } else if (array.size() == 1) {
        if (array[0].IsString()) parsed_headers.insert({kv.first, array[0].ToStringChecked()});
      } else {
        std::vector<std::string> vecs;
        std::string str;
        for (const auto& arr : array) {
          if (arr.IsString()) vecs.push_back(arr.ToStringChecked());
        }
        for (size_t i = 0; i < vecs.size(); i++) {
          str += vecs[i];
          if (i != vecs.size() - 1) str += ",";
        }
        if (vecs.size() > 0) parsed_headers.insert({kv.first, str});
      }
    }
  }
}

bool HttpNetwork::ParserRequestHeadersAndParameters(const footstone::value::HippyValue& value,
                                                    std::unordered_map<std::string, std::string>& parsed_headers,
                                                    std::unordered_map<std::string, std::string>& parsed_parameters) {
  footstone::value::HippyValue::HippyValueArrayType parameters;
  bool ret = value.ToArray(parameters);
  if (!ret) return false;
  if (parameters.size() != 1) return false;

  footstone::value::HippyValue::HippyValueObjectType objects;
  ret = parameters[0].ToObject(objects);
  if (!ret) return false;
  for (const auto& kv : objects) {
    if (kv.first == kHttpHeaders) {
      ParserRequestHeaders(kv.second, parsed_headers);
      continue;
    }
    if (kv.second.IsString()) parsed_parameters.insert({kv.first, kv.second.ToStringChecked()});
  }
  return true;
}

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
