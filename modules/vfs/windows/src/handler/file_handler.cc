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

#include "vfs/handler/file_handler.h"

#include "footstone/task.h"
#include "vfs/file.h"

namespace hippy {
inline namespace vfs {

void FileHandler::RequestUntrustedContent(std::shared_ptr<RequestJob> request, std::shared_ptr<JobResponse> response,
                                          std::function<std::shared_ptr<UriHandler>()> next) {
  string_view uri = request->GetUri();
  bool ret = HippyFile::ReadFile(uri, response->GetContent(), false);
  if (ret) {
    response->SetRetCode(UriHandler::RetCode::Success);
  } else {
    response->SetRetCode(UriHandler::RetCode::Failed);
  }
  auto next_handler = next();
  if (next_handler) {
    next_handler->RequestUntrustedContent(request, response, next);
  }
}

void FileHandler::RequestUntrustedContent(std::shared_ptr<RequestJob> request,
                                          std::function<void(std::shared_ptr<JobResponse>)> cb,
                                          [[maybe_unused]] std::function<std::shared_ptr<UriHandler>()> next) {
  string_view path = request->GetUri();
  auto new_cb = [orig_cb = cb](std::shared_ptr<JobResponse> response) { orig_cb(response); };
  LoadByFile(path, request, new_cb);
}

void FileHandler::LoadByFile(const string_view& path, [[maybe_unused]] std::shared_ptr<RequestJob> request,
                             std::function<void(std::shared_ptr<JobResponse>)> cb) {
  auto runner = runner_.lock();
  if (!runner) {
    cb(std::make_shared<JobResponse>(UriHandler::RetCode::DelegateError));
    return;
  }
  runner->PostTask([path, cb] {
    UriHandler::bytes content;
    bool ret = HippyFile::ReadFile(path, content, false);
    if (ret) {
      cb(std::make_shared<JobResponse>(hippy::JobResponse::RetCode::Success, "",
                                       std::unordered_map<std::string, std::string>{}, std::move(content)));
    } else {
      cb(std::make_shared<JobResponse>(hippy::JobResponse::RetCode::Failed));
    }
  });
}

}  // namespace vfs
}  // namespace hippy