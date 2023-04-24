//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/object.h"
#include "core/common/time.h"
#include "core/support/gesture/event/pointer_data.h"

namespace tdfcore {
/**
 * 用以生成 pointer id, 记录同一个 pointer 事件上一次点击的位置和时间
 */
class PointerState : public Object {
 public:
  ~PointerState() override = default;
  PointerState(TPoint point, TDuration time) : last_position_(point), last_timestamp_(time) {}

  /**
   * @brief
   * @return 同一个 pointer 事件上，与上一次点击位置的差值
   */
  TPoint DeltaTo(const TPoint &);

  /**
   *
   * @return 获取到这次生成的 pointer id, 生成规则是累计计算 pointer 事件次数
   */
  constexpr PointerID GetPointer() const { return pointer_; }

  /**
   * @brief 设置最后一次点击的位置
   * @param last_position 点击位置
   */
  void SetLastPosition(const TPoint &last_position) { last_position_ = last_position; }

  /**
   * @brief
   * @return 获取最后一次点击的位置
   */
  constexpr TPoint GetLastPosition() { return last_position_; }

  /**
   * @brief 设置最后一次点击的时间
   * @param last_timestamp 点击时间
   */
  void SetLastTimesTamp(TDuration last_timestamp) { last_timestamp_ = last_timestamp; }

  /**
   *
   * @return 获取最后一次的点击时间
   */
  constexpr TDuration GetLastTimesTamp() { return last_timestamp_; }

  /**
   * 开始创建新的手势 id
   */
  void StartNewPointer();

 private:
  static int64_t pointer_count_;

  PointerID pointer_ = 0;
  TPoint last_position_;
  TDuration last_timestamp_;
};
}  // namespace tdfcore
