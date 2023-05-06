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

#include "vfs/handler/http_handler.h"

#include "curl/curl_wrapper.h"
#include "footstone/deserializer.h"
#include "footstone/hippy_value.h"
#include "footstone/logging.h"
#include "footstone/task.h"
#include "vfs/handler/http_request.h"
#include "vfs/handler/http_response.h"

namespace hippy {
inline namespace vfs {

constexpr char kHttpHeaders[] = "headers";

void HttpHandler::ParsedHeaders(const footstone::value::HippyValue& headers,
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

bool HttpHandler::ParserParameters(const footstone::value::HippyValue& value,
                                   std::unordered_map<std::string, std::string>& parsed_parameters,
                                   std::unordered_map<std::string, std::string>& parsed_headers) {
  footstone::value::HippyValue::HippyValueArrayType parameters;
  bool ret = value.ToArray(parameters);
  if (!ret) return false;
  if (parameters.size() != 1) return false;

  footstone::value::HippyValue::HippyValueObjectType objects;
  ret = parameters[0].ToObject(objects);
  if (!ret) return false;
  for (const auto& kv : objects) {
    if (kv.first == kHttpHeaders) {
      ParsedHeaders(kv.second, parsed_headers);
      continue;
    }
    if (kv.second.IsString()) parsed_parameters.insert({kv.first, kv.second.ToStringChecked()});
  }
  return true;
}

void HttpHandler::RequestUntrustedContent(std::shared_ptr<RequestJob> request, std::shared_ptr<JobResponse> response,
                                          std::function<std::shared_ptr<UriHandler>()> next) {
  string_view url = request->GetUri();
  LoadByCurl(url, response);
  auto next_handler = next();
  if (next_handler) {
    next_handler->RequestUntrustedContent(request, response, next);
  }
}

void HttpHandler::RequestUntrustedContent(std::shared_ptr<RequestJob> request,
                                          std::function<void(std::shared_ptr<JobResponse>)> cb,
                                          [[maybe_unused]] std::function<std::shared_ptr<UriHandler>()> next) {
  string_view url = request->GetUri();
  // TODO(chareeshen): cookie 传递可能需要重构
  auto meta = request->GetMeta();
  std::string cookie_string;
  if (meta.find("COOKIE") != meta.end()) {
    cookie_string = meta.find("COOKIE")->second;
  }

  auto buffer = request->GetBuffer();
  if (!buffer.empty()) {
    auto new_cb = [orig_cb = cb](std::shared_ptr<JobResponse> response) { orig_cb(response); };
    std::unordered_map<std::string, std::string> parameters;
    std::unordered_map<std::string, std::string> headers;
    footstone::HippyValue hippy_value;
    footstone::Deserializer deserializer(reinterpret_cast<const uint8_t*>(buffer.c_str()), buffer.length());
    deserializer.ReadHeader();
    deserializer.ReadValue(hippy_value);
    auto ret = ParserParameters(hippy_value, parameters, headers);
    if (!cookie_string.empty()) {
      parameters.insert({"COOKIE", cookie_string});
    }

    if (ret) {
      LoadByCurl(url, parameters, headers, new_cb);
    }
  } else {
    auto new_cb = [orig_cb = cb](std::shared_ptr<JobResponse> response) { orig_cb(response); };
    LoadByCurl(url, new_cb);
  }
}

static size_t WriteHeaderCallback(void* content, size_t size, size_t nmemb, void* userp) {
  auto read_buffer = static_cast<std::vector<uint8_t>*>(userp);
  read_buffer->insert(read_buffer->end(), static_cast<uint8_t*>(content),
                      static_cast<uint8_t*>(content) + size * nmemb);
  return size * nmemb;
}

static size_t WriteBodyCallback(void* content, size_t size, size_t nmemb, void* userp) {
  auto read_buffer = static_cast<std::vector<uint8_t>*>(userp);
  read_buffer->insert(read_buffer->end(), static_cast<uint8_t*>(content),
                      static_cast<uint8_t*>(content) + size * nmemb);
  return size * nmemb;
}

void HttpHandler::LoadByCurl(const string_view& url, std::function<void(std::shared_ptr<JobResponse>)> cb) {
  auto runner = runner_.lock();
  if (!runner) {
    cb(std::make_shared<JobResponse>(UriHandler::RetCode::DelegateError));
    return;
  }
  runner->PostTask([url, cb] {
    std::vector<uint8_t> read_buffer;
    CurlWrapper curl_wrapper;
    auto ret = curl_wrapper.Initialize();
    if (!ret) {
      cb(std::make_shared<JobResponse>(hippy::JobResponse::RetCode::Failed));
      return;
    }

    auto curl_ptr = curl_wrapper.GetCurlRawPointer();
    curl_easy_setopt(curl_ptr, CURLOPT_URL, url.latin1_value().c_str());
    curl_easy_setopt(curl_ptr, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl_ptr, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl_ptr, CURLOPT_NOBODY, 0);
    curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, WriteBodyCallback);
    curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, &read_buffer);
    auto res = curl_easy_perform(curl_ptr);

    if (res == 0) {
      UriHandler::bytes content{read_buffer.begin(), read_buffer.end()};
      cb(std::make_shared<JobResponse>(hippy::JobResponse::RetCode::Success, "",
                                       std::unordered_map<std::string, std::string>{}, std::move(content)));
    } else {
      cb(std::make_shared<JobResponse>(hippy::JobResponse::RetCode::Failed));
    }
  });
}

// TODO(charleeshen): 302 请求需要处理。 获取 cookie 需要处理
void HttpHandler::LoadByCurl(const string_view& url, const std::unordered_map<std::string, std::string>& parameters,
                             const std::unordered_map<std::string, std::string>& headers,
                             std::function<void(std::shared_ptr<JobResponse>)> cb) {
  auto runner = runner_.lock();
  if (!runner) {
    cb(std::make_shared<JobResponse>(UriHandler::RetCode::DelegateError));
    return;
  }

  HttpRequest request(url, headers, parameters);
  request.SetConnectionTimeout(10 * 1000);
  request.SetReadTimeout(16 * 1000);
  request.SetUseCache(false);
  runner->PostTask([request, cb] {
    std::vector<uint8_t> read_header_buffer;
    std::vector<uint8_t> read_body_buffer;
    CurlWrapper curl_wrapper;
    auto ret = curl_wrapper.Initialize();
    if (!ret) {
      cb(std::make_shared<JobResponse>(hippy::JobResponse::RetCode::Failed));
      return;
    }

    auto curl_ptr = curl_wrapper.GetCurlRawPointer();
    curl_easy_setopt(curl_ptr, CURLOPT_URL, request.GetUrl().latin1_value().c_str());
    curl_easy_setopt(curl_ptr, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_ptr, CURLOPT_CUSTOMREQUEST, request.RequestMethod().c_str());
    if (!request.GetUseCache()) curl_easy_setopt(curl_ptr, CURLOPT_FRESH_CONNECT, 1L);
    curl_easy_setopt(curl_ptr, CURLOPT_CONNECTTIMEOUT_MS, request.GetConnectionTimeout());
    curl_easy_setopt(curl_ptr, CURLOPT_TIMEOUT, request.GetReadTimeout());
    curl_easy_setopt(curl_ptr, CURLOPT_FOLLOWLOCATION, 1L);
    if (!request.GetCookie().empty()) curl_easy_setopt(curl_ptr, CURLOPT_COOKIE, request.GetCookie().c_str());
    if (!request.EnableFollowLocation()) curl_easy_setopt(curl_ptr, CURLOPT_FOLLOWLOCATION, 0L);

    curl_slist* curl_headers{nullptr};
    auto request_headers = request.RequestHeaders();
    for (const auto& [key, value] : request_headers) {
      std::string header_string = key;
      header_string += ": ";
      header_string += value;
      curl_headers = curl_slist_append(curl_headers, header_string.c_str());
    }
    curl_easy_setopt(curl_ptr, CURLOPT_HTTPHEADER, curl_headers);

    auto body = request.RequestBody();
    if (!body.empty()) {
      curl_easy_setopt(curl_ptr, CURLOPT_POSTFIELDS, body.c_str());
    }

    auto user_agent = request.RequestUserAgent();
    if (!user_agent.empty()) {
      curl_easy_setopt(curl_ptr, CURLOPT_USERAGENT, user_agent.c_str());
    }

    curl_easy_setopt(curl_ptr, CURLOPT_HEADERFUNCTION, WriteHeaderCallback);
    curl_easy_setopt(curl_ptr, CURLOPT_HEADERDATA, &read_header_buffer);
    curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, WriteBodyCallback);
    curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, &read_body_buffer);
    auto res = curl_easy_perform(curl_ptr);
    curl_slist_free_all(curl_headers);

    if (res == CURLE_OK) {
      HttpResponse response(read_header_buffer, read_body_buffer);
      std::string buffer;
      response.Parser();
      response.ResponseBuffer(buffer);
      cb(std::make_shared<JobResponse>(hippy::JobResponse::RetCode::Success, "",
                                       std::unordered_map<std::string, std::string>{}, std::move(buffer)));
    } else {
      FOOTSTONE_DLOG(INFO) << "curl_easy_perform() failed: " << curl_easy_strerror(res);
      std::string error_message = "Load remote resource failed: ";
      error_message += curl_easy_strerror(res);
      cb(std::make_shared<JobResponse>(hippy::JobResponse::RetCode::Failed, error_message.c_str(),
                                       std::unordered_map<std::string, std::string>{}, ""));
    }
  });
}

void HttpHandler::LoadByCurl(const string_view& url, const std::shared_ptr<JobResponse>& response) {
  std::vector<uint8_t> read_buffer;
  CurlWrapper curl_wrapper;
  auto ret = curl_wrapper.Initialize();
  if (!ret) {
    response->SetRetCode(hippy::JobResponse::RetCode::Failed);
    return;
  }

  auto curl_ptr = curl_wrapper.GetCurlRawPointer();
  curl_easy_setopt(curl_ptr, CURLOPT_URL, url.latin1_value().c_str());
  curl_easy_setopt(curl_ptr, CURLOPT_CUSTOMREQUEST, "GET");
  curl_easy_setopt(curl_ptr, CURLOPT_HEADER, 0);
  curl_easy_setopt(curl_ptr, CURLOPT_NOBODY, 0);
  curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, WriteBodyCallback);
  curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, &read_buffer);
  auto res = curl_easy_perform(curl_ptr);

  if (res == 0) {
    UriHandler::bytes content{read_buffer.begin(), read_buffer.end()};
    response->SetRetCode(hippy::JobResponse::RetCode::Success);
    response->SetContent(std::move(content));
  } else {
    response->SetRetCode(hippy::JobResponse::RetCode::Failed);
  }
}

}  // namespace vfs
}  // namespace hippy
