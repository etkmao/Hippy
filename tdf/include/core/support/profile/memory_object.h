//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <unordered_map>
#include "core/common/time.h"

namespace tdfcore {

/**
 * @brief 记录源代码位置信息
 */
struct SourceCodeLocation {
  // 源文件
  const char *file = nullptr;
  // 代码行数
  int64_t line = 0;
};

/**
 * @brief 内存中的一个对象
 */
struct MemoryObject {
  // 指针
  const void *pointer = nullptr;
  // 占用内存量
  int64_t bytes = 0;
  // 类型
  const char *type = nullptr;
  // 对象类别，可能为空
  const char *category = nullptr;
  // 在源代码中的何处为该对象分配了内存
  SourceCodeLocation alloc_location;

  /**
    * @brief 哈希值
    */
  int64_t Hash() const { return reinterpret_cast<int64_t>(pointer); }
};

using MemoryObjectMap = std::unordered_map<const void *, MemoryObject>;

enum class MemoryEventType : uint8_t {
  kAlloc = 0,
  kFree = 1,
};

/**
 * @brief 内存上报的原始事件数据结构
 */
struct MemoryEvent {
  // 事件类型：分配/释放
  MemoryEventType type = MemoryEventType::kAlloc;
  // 时间戳
  RealTimePoint timestamp;
  // 内存对象的详细信息
  MemoryObject object;
};

}  // namespace tdfcore
