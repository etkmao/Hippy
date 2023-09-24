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

#include "include/footstone/task.h"

#include <utility>

namespace footstone {
inline namespace runner {

std::atomic<uint32_t> Task::g_next_task_id = 1;

Task::Task() : Task(nullptr) {}

Task::Task(std::function<void()> exec_unit) : unit_(std::move(exec_unit)) {
  id_ = g_next_task_id.fetch_add(1);
}

} // namespace runner
} // namespace footstone

// TODO:
static int sCnt = 0;
void XXXLogTaskFunction(const char* str, double dt, double start) {
    printf("--- xxx task, %4d, %-60s, dt: %4.6lf, start: %6.f, %s, %s, %s, %s, %s\n", ++sCnt, str?str:"", dt, start,
           dt>0.1f?  ">0.1ms": "      ",
           dt>1.f?   ">1ms":   "    ",
           dt>10.f?  ">10ms":  "     ",
           dt>50.f?  ">50ms":  "     ",
           dt>100.f? ">100ms": "      ");
}
