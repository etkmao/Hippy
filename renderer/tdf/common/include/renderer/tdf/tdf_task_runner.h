/**
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

#include "dom/dom_manager.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wc++98-compat-extra-semi"
#pragma clang diagnostic ignored "-Wignored-qualifiers"
#pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wfloat-conversion"
#pragma clang diagnostic ignored "-Wshadow"
//#pragma clang diagnostic ignored "-Wdeprecated-copy-with-dtor"
//#pragma clang diagnostic ignored "-Wdeprecated-copy"
#include "core/engine/schedule/task_runner.h"
#include "core/tdfi/shell.h"
#pragma clang diagnostic pop

namespace hippy {
inline namespace render {
inline namespace tdf {

class TDFTaskRunner
    : public tdfcore::TaskRunner { //, public std::enable_shared_from_this<TDFTaskRunner>

 public:
  ~TDFTaskRunner() override;

  TDFTaskRunner(const char *name, const std::shared_ptr<tdfcore::EngineClock> &clock,
                std::shared_ptr<tdfcore::Shell> shell,
                std::shared_ptr<DomManager> dom_manager);

  void PostTask(const std::shared_ptr<tdfcore::Task> &task) override;

  void PostTask(const std::shared_ptr<tdfcore::Task> &task, const tdfcore::TDuration &delay) override;

 private:

  std::weak_ptr<tdfcore::Shell> shell_;
  std::weak_ptr<DomManager> dom_manager_;
};

}  // namespace tdf
}  // namespace render
}  // namespace hippy
