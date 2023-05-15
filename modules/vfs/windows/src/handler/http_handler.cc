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

#include "footstone/deserializer.h"
#include "footstone/hippy_value.h"
#include "footstone/logging.h"
#include "footstone/macros.h"
#include "footstone/task.h"
#include "vfs/handler/http_request.h"
#include "vfs/handler/http_response.h"

namespace hippy {
inline namespace vfs {

HttpHandler::HttpHandler() { internet_handle_ = InternetOpen("HTTP", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0); }

HttpHandler::~HttpHandler() {
  if (internet_handle_) InternetCloseHandle(internet_handle_);
}

static void LogLastErrorMessage() {
  DWORD error_code = GetLastError();
  LPSTR error_message;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
                error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&error_message, 0, nullptr);
  if (error_message != nullptr) {
    FOOTSTONE_DLOG(INFO) << "windows error code: " << error_code << ", error message: " << error_message;
  }
  LocalFree(error_message);
}

bool static CreateUrlComponents(const std::string& url, URL_COMPONENTS& url_components) {
  url_components.dwStructSize = sizeof(URL_COMPONENTS);
  url_components.lpszHostName = new char[256];
  url_components.dwHostNameLength = 256;
  url_components.lpszUrlPath = new char[1024];
  url_components.dwUrlPathLength = 1024;
  return InternetCrackUrl(url.c_str(), url.size(), 0, &url_components);
}

void static FreeUrlComponents(URL_COMPONENTS& url_components) {
  delete[] url_components.lpszHostName;
  delete[] url_components.lpszUrlPath;
}

static void FreeRequestHandle(HINTERNET connection_handle, HINTERNET request_handle, URL_COMPONENTS& url_components,
                              bool error_log = true) {
  if (!request_handle) InternetCloseHandle(request_handle);
  if (!connection_handle) InternetCloseHandle(connection_handle);
  FreeUrlComponents(url_components);
  if (error_log) LogLastErrorMessage();
}

void HttpHandler::RequestUntrustedContent(std::shared_ptr<RequestJob> request, std::shared_ptr<JobResponse> response,
                                          std::function<std::shared_ptr<UriHandler>()> next) {
  LoadUriContent(request, response);
  auto next_handler = next();
  if (next_handler) {
    next_handler->RequestUntrustedContent(request, response, next);
  }
}

void HttpHandler::RequestUntrustedContent(std::shared_ptr<RequestJob> request,
                                          std::function<void(std::shared_ptr<JobResponse>)> cb,
                                          [[maybe_unused]] std::function<std::shared_ptr<UriHandler>()> next) {
  string_view uri = request->GetUri();
  auto new_cb = [orig_cb = cb](std::shared_ptr<JobResponse> response) { orig_cb(response); };
  LoadUriContent(request, new_cb);
}

void HttpHandler::LoadUriContent(const std::shared_ptr<RequestJob>& request,
                                 const std::shared_ptr<JobResponse>& response) {
  string_view uri = request->GetUri();
  URL_COMPONENTS url_components = {0};
  if (!CreateUrlComponents(uri.latin1_value(), url_components)) {
    FreeRequestHandle(nullptr, nullptr, url_components, true);
    response->SetRetCode(hippy::JobResponse::RetCode::Failed);
    return;
  }
  auto handle_connect = InternetConnect(internet_handle_, url_components.lpszHostName, url_components.nPort, nullptr,
                                        nullptr, INTERNET_SERVICE_HTTP, 0, 0);
  if (handle_connect == nullptr) {
    FreeRequestHandle(nullptr, nullptr, url_components, true);
    response->SetRetCode(hippy::JobResponse::RetCode::Failed);
    return;
  }

  auto handle_request = HttpOpenRequest(handle_connect, "GET", url_components.lpszUrlPath, nullptr, nullptr, nullptr,
                                        INTERNET_FLAG_RELOAD, 0);
  if (handle_request == nullptr) {
    FreeRequestHandle(handle_connect, nullptr, url_components, true);
    response->SetRetCode(hippy::JobResponse::RetCode::Failed);
    return;
  }

  if (!HttpSendRequest(handle_request, nullptr, 0, nullptr, 0)) {
    FreeRequestHandle(handle_connect, handle_request, url_components, true);
    response->SetRetCode(hippy::JobResponse::RetCode::Failed);
    return;
  }

  std::vector<char> buffer, recv_buffer;
  recv_buffer.resize(1024);
  DWORD bytes_read = 0;
  while (InternetReadFile(handle_request, &recv_buffer[0], 1024, &bytes_read) && bytes_read > 0) {
    buffer.insert(buffer.end(), &recv_buffer[0], &recv_buffer[0] + bytes_read);
  }
  UriHandler::bytes content{buffer.begin(), buffer.end()};
  response->SetRetCode(hippy::JobResponse::RetCode::Success);
  response->SetContent(std::move(content));
  FreeRequestHandle(handle_connect, handle_request, url_components, false);
  return;
}

static void QueryInfo(const HINTERNET& handle_request, const DWORD query_flag, std::vector<char>& buffer) {
  DWORD size = 0;
  HttpQueryInfo(handle_request, query_flag, nullptr, &size, nullptr);
  buffer.resize(size);
  HttpQueryInfo(handle_request, query_flag, &buffer[0], &size, nullptr);
}

void HttpHandler::LoadUriContent(const std::shared_ptr<RequestJob>& request,
                                 std::function<void(std::shared_ptr<JobResponse>)> callback) {
  auto runner = runner_.lock();
  if (!runner) {
    callback(std::make_shared<JobResponse>(UriHandler::RetCode::DelegateError));
    return;
  }
  auto uri = request->GetUri();
  auto meta = request->GetMeta();
  HttpRequest http_request(uri, meta);
  http_request.SetConnectionTimeout(10 * 1000);
  http_request.SetReadTimeout(16 * 1000);
  http_request.SetUseCache(false);
  runner->PostTask([WEAK_THIS, http_request, callback] {
    DEFINE_AND_CHECK_SELF(HttpHandler);
    string_view uri = http_request.GetUri();
    URL_COMPONENTS url_components = {0};
    if (CreateUrlComponents(uri.latin1_value(), url_components)) {
      auto handle_connect = InternetConnect(self->internet_handle_, url_components.lpszHostName, url_components.nPort,
                                            nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
      if (handle_connect == nullptr) return FreeRequestHandle(nullptr, nullptr, url_components, true);
      DWORD flags = 0;
      if (!http_request.EnableFollowLocation()) flags = flags | INTERNET_FLAG_NO_AUTO_REDIRECT;       // redirect policy
      if (url_components.nPort == INTERNET_DEFAULT_HTTPS_PORT) flags = flags | INTERNET_FLAG_SECURE;  // https policy
      auto method = http_request.RequestMethod();
      auto handle_request = HttpOpenRequest(handle_connect, method.c_str(), url_components.lpszUrlPath, nullptr,
                                            nullptr, nullptr, flags, 0);
      if (handle_request == nullptr) return FreeRequestHandle(handle_connect, nullptr, url_components, true);
      // set connect time out
      DWORD send_timeout = http_request.GetConnectionTimeout();
      InternetSetOption(handle_request, INTERNET_OPTION_SEND_TIMEOUT, &send_timeout, sizeof(send_timeout));
      // set read time out
      DWORD read_timeout = http_request.GetReadTimeout();  // 5 seconds
      InternetSetOption(handle_request, INTERNET_OPTION_RECEIVE_TIMEOUT, &read_timeout, sizeof(read_timeout));

      auto ret = true;
      // set request headers
      if (!http_request.RequestHeaders().empty())
        HttpAddRequestHeaders(handle_request, http_request.RequestHeaders().c_str(), -1L, HTTP_ADDREQ_FLAG_ADD);
      // send request
      if (!http_request.RequestBody().empty()) {
        auto body = http_request.RequestBody();
        ret = HttpSendRequest(handle_request, nullptr, 0, body.data(), body.size());
      } else {
        ret = HttpSendRequest(handle_request, nullptr, 0, nullptr, 0);
      }
      if (!ret) return FreeRequestHandle(handle_connect, handle_request, url_components, true);

      // header buffer
      std::vector<char> header_buffer;
      QueryInfo(handle_request, HTTP_QUERY_RAW_HEADERS_CRLF, header_buffer);

      // read body
      std::vector<char> body_buffer, recv_buffer;
      recv_buffer.resize(1024);
      DWORD bytes_read = 0;
      while (InternetReadFile(handle_request, &recv_buffer[0], 1024, &bytes_read) && bytes_read > 0) {
        body_buffer.insert(body_buffer.end(), &recv_buffer[0], &recv_buffer[0] + bytes_read);
      }
      FreeRequestHandle(handle_connect, handle_request, url_components, false);

      HttpResponse response(header_buffer, body_buffer);
      response.Parser();
      auto headers = response.ResponseHeaders();
      auto content = response.ResponseBody();
      callback(std::make_shared<JobResponse>(hippy::JobResponse::RetCode::Success, "", headers, std::move(content)));
    }
  });
  return;
}

}  // namespace vfs
}  // namespace hippy
