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

#include <atomic>
#include <cstdint>
#include <functional>

// TODO:
extern clock_t gXXXBaseTime;
//#define XXX_LOG_TASK_BEGIN
//#define XXX_LOG_TASK_END(str)
#define XXX_LOG_TASK_BEGIN \
    clock_t ttStart = clock();
#define XXX_LOG_TASK_END(str) \
    double ttDuration = (double)(clock() - ttStart)/CLOCKS_PER_SEC*1000.f; \
    double ttDStart = (double)(ttStart - gXXXBaseTime)/CLOCKS_PER_SEC*1000.f; \
    XXXLogTaskFunction(str,ttDuration,ttDStart);
extern void XXXLogTaskFunction(const char* str, double dt, double start);

namespace footstone {
inline namespace runner {

class Task {
 public:
  Task();
  explicit Task(std::function<void()> unit);
  ~Task() = default;

  inline uint32_t GetId() { return id_; }
  inline void SetExecUnit(std::function<void()> unit) { unit_ = unit; }
  inline void Run() {
      XXX_LOG_TASK_BEGIN
    if (unit_) {
      unit_();
    }
      XXX_LOG_TASK_END("run")
  }

 private:
  static std::atomic<uint32_t> g_next_task_id;

  std::atomic<uint32_t> id_{};
  std::function<void()> unit_;  // A unit of work to be processed
};

}  // namespace runner
}  // namespace footstone
