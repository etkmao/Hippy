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

#include "renderer/tdf/tdf_task_runner.h"

//#include <cstdint>
//#include <iostream>
//#include <utility>
//
//#include "core/engine/schedule/scheduler.h"
//#include "core/engine/schedule/task_runner_checker.h"
//#include "dom/root_node.h"
//#include "footstone/logging.h"
//#include "renderer/tdf/viewnode/view_node.h"
//#include "renderer/tdf/viewnode/embedded_view_node.h"
//#include "renderer/tdf/viewnode/image_view_node.h"
//#include "renderer/tdf/viewnode/list_view_node.h"
//#include "renderer/tdf/viewnode/modal_view_node.h"
//#include "renderer/tdf/viewnode/refresh_wrapper_node.h"
//#include "renderer/tdf/viewnode/root_view_node.h"
//#include "renderer/tdf/viewnode/scroll_view_node.h"
//#include "renderer/tdf/viewnode/text_input_node.h"
//#include "renderer/tdf/viewnode/text_view_node.h"
//#include "renderer/tdf/viewnode/view_names.h"
//#include "renderer/tdf/viewnode/view_pager_node.h"

//#include "core/common/macro.h"
//#include "footstone/macros.h"

namespace hippy {
inline namespace render {
inline namespace tdf {

TDFTaskRunner::~TDFTaskRunner() {

}

TDFTaskRunner::TDFTaskRunner(const char *name, const std::shared_ptr<tdfcore::EngineClock> &clock,
                             std::shared_ptr<tdfcore::Shell> shell,
                             std::shared_ptr<DomManager> dom_manager)
: tdfcore::TaskRunner(name, clock), shell_(shell), dom_manager_(dom_manager) {

}

void TDFTaskRunner::PostTask(const std::shared_ptr<tdfcore::Task> &task) {
  std::vector<std::function<void()>> ops;
  ops.emplace_back([WEAK_THIS, task] {
    DEFINE_AND_CHECK_SELF(TDFTaskRunner)
    auto notifier = self->shell_.lock()->GetTaskDriverEventNotifier();
    notifier->Notify(nullptr, tdfcore::Scheduler::TaskDriverEvent::kEnterRunLoop);
    task->Run();
    //notifier->Notify(nullptr, tdfcore::Scheduler::TaskDriverEvent::kLeaveRunLoop);
  });
  dom_manager_.lock()->PostTask(hippy::Scene(std::move(ops)));
}

void TDFTaskRunner::PostTask(const std::shared_ptr<tdfcore::Task> &task, const tdfcore::TDuration &delay) {
  std::vector<std::function<void()>> ops;
  ops.emplace_back([WEAK_THIS, task] {
    DEFINE_AND_CHECK_SELF(TDFTaskRunner)
//    auto notifier = self->shell_.lock()->GetTaskDriverEventNotifier();
//    notifier->Notify(nullptr, tdfcore::Scheduler::TaskDriverEvent::kEnterRunLoop);
    task->Run();
    //notifier->Notify(nullptr, tdfcore::Scheduler::TaskDriverEvent::kLeaveRunLoop);
  });
  dom_manager_.lock()->PostDelayedTask(hippy::Scene(std::move(ops)), static_cast<uint64_t>(delay.count()));
}

#undef GET_SHELL
#undef FOR_EACH_TEXT_NODE
}  // namespace tdf
}  // namespace render
}  // namespace hippy
