//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/recognizer/multi_drag_gesture_recognizer.h"

namespace tdfcore {
/**
 * @brief 多指长按滑动手势，配合[DelayedPointerState]使用。功能与[ImmediateMultiDragGestureRecognizer]类似，
 * 区别在于[DelayedMultiDragGestureRecognizer]不需要靠滑动距离来从竞技场中胜出，而是通过按下后停留一段时间后获胜，
 * 其中有两个条件：1.手指按下后停留时长满足[kLongPressTimeout]以上；2.停留期间滑动距离不超过[kTouchSlop]。
 *
 * 与[LongPressGestureRecognizer]也很像，都是长按后拖动，但[DelayedMultiDragGestureRecognizer]可以监听多手势，但是
 * [LongPressGestureRecognizer]只能监听单手势
 */
class DelayedMultiDragGestureRecognizer : public MultiDragGestureRecognizer {
  TDF_REFF_CLASS_META(DelayedMultiDragGestureRecognizer)
 public:
  DelayedMultiDragGestureRecognizer() = default;
  std::shared_ptr<MultiDragPointerState> CreateNewPointerState(const PointerData &data) const override;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::DelayedMultiDragGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::DelayedMultiDragGestureRecognizer)