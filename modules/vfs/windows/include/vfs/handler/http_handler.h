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

#include "footstone/hippy_value.h"
#include "footstone/string_view.h"
#include "footstone/task_runner.h"
#include "vfs/handler/uri_handler.h"

namespace hippy {
inline namespace vfs {

class HttpHandler : public UriHandler {
 public:
  using string_view = footstone::string_view;
  using TaskRunner = footstone::TaskRunner;

  HttpHandler() = default;
  virtual ~HttpHandler() override = default;

  inline void SetWorkerTaskRunner(std::weak_ptr<TaskRunner> runner) { runner_ = runner; }

  virtual void RequestUntrustedContent(std::shared_ptr<RequestJob> request, std::shared_ptr<JobResponse> response,
                                       std::function<std::shared_ptr<UriHandler>()> next) override;
  virtual void RequestUntrustedContent(std::shared_ptr<RequestJob> request,
                                       std::function<void(std::shared_ptr<JobResponse>)> cb,
                                       std::function<std::shared_ptr<UriHandler>()> next) override;

 private:
  void ParsedHeaders(const footstone::HippyValue& headers,
                     std::unordered_map<std::string, std::string>& parsed_headers);
  bool ParserParameters(const footstone::HippyValue& value,
                        std::unordered_map<std::string, std::string>& parsed_parameters,
                        std::unordered_map<std::string, std::string>& parsed_headers);
  void LoadByCurl(const string_view& url, std::function<void(std::shared_ptr<JobResponse>)> cb);
  void LoadByCurl(const string_view& url, const std::unordered_map<std::string, std::string>& parameters,
                  const std::unordered_map<std::string, std::string>& headers,
                  std::function<void(std::shared_ptr<JobResponse>)> cb);
  void LoadByCurl(const string_view& url, const std::shared_ptr<JobResponse>& response);

 private:
  std::weak_ptr<TaskRunner> runner_;
};

}  // namespace vfs
}  // namespace hippy
