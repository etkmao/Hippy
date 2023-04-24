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

#include "footstone/serializer.h"
#include "vfs/handler/http_handler.h"
#include "vfs/handler/uri_handler.h"

namespace hippy {
inline namespace framework {
inline namespace windows {
inline namespace adaptor {

using string_view = footstone::stringview::string_view;

HttpNetwork::HttpNetwork() : http_cookie_(std::make_shared<HttpCookie>()) {}

bool HttpNetwork::Initial() { return http_cookie_->Initial(); }

void HttpNetwork::Fetch(const std::shared_ptr<Context>& context, const std::string& url,
                        const footstone::value::HippyValue& request, std::function<void(std::string)> callback) {
  footstone::Serializer serializer;
  serializer.WriteHeader();
  serializer.WriteValue(request);
  std::pair<uint8_t*, size_t> buffer = serializer.Release();
  std::string content_string{reinterpret_cast<char*>(buffer.first), buffer.second};

  // parsed cookie
  std::unordered_map<std::string, std::string> meta;
  std::string cookie_string;
  std::vector<std::string> cookies;
  http_cookie_->GetCookies(url, cookies);
  for (size_t i = 0; i < cookies.size(); i++) {
    cookie_string += cookies[i];
    if (i != cookies.size()) cookie_string += ";";
  }
  if (!cookie_string.empty()) {
    meta.insert({"COOKIE", cookie_string});
  }

  auto uri_loader = context->GetUriLoader();
  auto request_job = std::make_shared<RequestJob>(string_view(url.c_str()), meta, uri_loader->GetWorkerManager(),
                                                  nullptr, std::move(content_string));
  std::function<void(std::shared_ptr<JobResponse>)> response_cb = [callback](const std::shared_ptr<JobResponse>& rsp) {
    if (rsp->GetRetCode() == hippy::JobResponse::RetCode::Success) {
      auto content = rsp->ReleaseContent();
      callback(content);
    } else {
      auto content = rsp->ReleaseContent();
      callback(content);
    }
  };
  uri_loader->RequestUntrustedContent(request_job, response_cb);
}

void HttpNetwork::GetCookie(const std::string& url, std::function<void(footstone::value::HippyValue)> callback) {
  http_cookie_->GetCookie(url, callback);
  return;
}

void HttpNetwork::SetCookie(const footstone::value::HippyValue& request) {
  std::string url, kv, expires;
  if (PrasedRequest(request, url, kv, expires)) {
    http_cookie_->SetCookie(url, kv, expires);
  }
  return;
}

bool HttpNetwork::PrasedRequest(const footstone::value::HippyValue& request, std::string& url, std::string& kv,
                                std::string& expires) {
  footstone::value::HippyValue::HippyValueArrayType array;
  if (!request.ToArray(array) || array.size() != 3) return false;
  if (!array[0].ToString(url) || !array[1].ToString(kv) || !array[2].ToString(expires)) return false;
  return true;
}

}  // namespace adaptor
}  // namespace windows
}  // namespace framework
}  // namespace hippy
