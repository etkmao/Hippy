//
// Copyright © 2020 com.tencent. All rights reserved.
//

#pragma once

#include "core/engine/schedule/task_runner.h"
#include "core/engine/schedule/task_runner_names.h"

#define TDF_RUNNER_WARNING(x)                                                                                  \
  if (::tdfcore::TaskRunner::GetCurrent() != nullptr && ::tdfcore::TaskRunner::GetCurrent()->GetName() != x) { \
    TDF_LOGE << "wrong TaskRunner, expect: " << x;                                                             \
  }
#define TDF_RUNNER_WARNING_UI TDF_RUNNER_WARNING(::tdfcore::kUITaskRunnerName)

#if TDF_RUNNER_CHECK_ENABLED

#  define TDF_RUNNER_CHECK(x) \
    assert(::tdfcore::TaskRunner::GetCurrent() != nullptr && ::tdfcore::TaskRunner::GetCurrent()->GetName() == x);

#  define TDF_RUNNER_CHECK_UI TDF_RUNNER_CHECK(::tdfcore::kUITaskRunnerName)
#  define TDF_RUNNER_CHECK_RASTER TDF_RUNNER_CHECK(::tdfcore::kRasterTaskRunnerName)
#  define TDF_RUNNER_CHECK_PLATFORM TDF_RUNNER_CHECK(::tdfcore::kPlatformTaskRunnerName)
#  define TDF_RUNNER_CHECK_IO TDF_RUNNER_CHECK(::tdfcore::kIOTaskRunnerName)

#  define TDF_CHECK_DEADLOCK_FOR_SCHEDULED_TASK(task) CheckDeadlockForScheduledTask(task)
#  define TDF_TRACE_THREAD_OF_TASK_RUNNER(task_runner) TraceThreadOfTaskRunner(task_runner)
#  define TDF_TRACE_FREE_TASK_RUNNER(task_runner) TraceFreeTaskRunner(task_runner)
#  define TDF_TRACE_POST_TASK(task_runner, task) TracePostTask(task_runner, task)
#  define TDF_TRACE_POP_TASK(task_runner, task) TracePopTask(task_runner, task)

namespace tdfcore {

bool CheckDeadlockForScheduledTask(const Task *task);
void TraceThreadOfTaskRunner(const TaskRunner *task_runner);
void TraceFreeTaskRunner(const TaskRunner *task_runner);
void TracePostTask(const TaskRunner *task_runner, const Task *task);
void TracePopTask(const TaskRunner *task_runner, const Task *task);

}  // namespace tdfcore

#else

#  define TDF_RUNNER_CHECK_UI TDF_RUNNER_WARNING_UI
#  define TDF_RUNNER_CHECK_RASTER
#  define TDF_RUNNER_CHECK_PLATFORM
#  define TDF_RUNNER_CHECK_IO

#  define TDF_CHECK_DEADLOCK_FOR_SCHEDULED_TASK(task) true
#  define TDF_TRACE_THREAD_OF_TASK_RUNNER(task_runner)
#  define TDF_TRACE_FREE_TASK_RUNNER(task_runner)
#  define TDF_TRACE_POST_TASK(task_runner, task)
#  define TDF_TRACE_POP_TASK(task_runner, task)

#endif
