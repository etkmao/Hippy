//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <chrono>
#include "core/common/macro.h"

namespace tdfcore {

enum class TimelineQueueItemType : uint8_t {
  kBegin = 0,
  kEnd = 1,
};

struct TimelineQueueItem {
  TimelineQueueItemType type;
  std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> timestamp;
  const char *label = nullptr;
  int64_t group_id = 0;
};

enum class MemoryQueueItemType : uint8_t {
  kAlloc = 0,
  kFree = 1,
};

struct MemoryAllocation {
  int64_t bytes = 0;
  const char *type = nullptr;
  const char *category = nullptr;
  const char *file = nullptr;
  int64_t line = 0;
};

struct MemoryQueueItem {
  MemoryQueueItemType type;
  const void *pointer = nullptr;
  std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> timestamp;
  MemoryAllocation allocation;
};

}  // namespace tdfcore
