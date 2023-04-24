#pragma once

#include <string>
#include <unordered_map>
#include "core/common/time.h"

namespace tdfcore {

/**
 * @brief 一个Task执行的时间信息
 */
struct TaskTimingZone {
  const char* task_runner;
  // Task真正被执行与预期被执行的时间延迟
  TDuration execution_delay;
  // Task本身执行的耗时
  TDuration execution_cost;
  // 时间戳
  RealTimePoint timestamp = RealTimePoint(TDuration(0));
};

using TaskTimingZoneMap = std::unordered_map<const char*, std::vector<TaskTimingZone>>;

}  // namespace tdfcore
