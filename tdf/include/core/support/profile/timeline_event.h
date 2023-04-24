//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <string>
#include "core/common/time.h"

namespace tdfcore {

enum class TimelineEventType {
  kInstant,
  kBegin,
  kEnd,
};

/**
 * @brief 一个事件（由函数调用、追踪打点等等方式所标记的事件）的详细时间线信息
 */
struct TimelineEvent {
  // 事件类型
  TimelineEventType type = TimelineEventType::kInstant;
  // 事件名称
  const char *label = nullptr;
  // 事件时间戳
  RealTimePoint timestamp = RealTimePoint(TDuration(0));
  // 唯一group_id，同一次绘制流程中的所有Event，具有相同的group_id
  int64_t group_id = 0;
  // 线程id
  uint32_t thread_id = 0;

  /**
   * @brief 序列化为JSON字符串
   */
  std::string ToJsonString() const;
};

/**
 * @brief 一个事件（由函数调用、追踪打点等等方式所标记的事件）的详细时间线信息
 */
struct TimelineZone {
  // 事件名称
  const char *label = nullptr;
  // 事件开始时间戳
  RealTimePoint timestamp_begin = RealTimePoint(TDuration(0));
  // 事件结束时间戳
  RealTimePoint timestamp_end = RealTimePoint(TDuration(0));
  // 唯一group_id，同一次绘制流程中的所有Event，具有相同的group_id
  int64_t group_id = 0;
  // 线程id
  uint32_t thread_id = 0;

  /**
   * @brief 事件耗时
   */
  constexpr TDuration Cost() const { return timestamp_end - timestamp_begin; }

  /**
   * @brief 是否为瞬时事件
   * @detial 区别于有开始和结束时间戳的事件，瞬时事件没有事件耗时的概念，只有一个时间戳。
   */
  constexpr bool IsInstant() const { return Cost() == TDuration(0); }

  /**
   * @brief 序列化为JSON字符串
   */
  std::string ToJsonString() const;
};

}  // namespace tdfcore
