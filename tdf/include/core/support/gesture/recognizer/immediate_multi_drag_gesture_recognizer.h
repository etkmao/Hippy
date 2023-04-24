//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/recognizer/multi_drag_gesture_recognizer.h"

namespace tdfcore {
/**
 * @brief 监听任意方向手势滑动事件，配合[ImmediatePointerState]使用，当滑动距离超过kTouchSlop时竞技成功
 */
class ImmediateMultiDragGestureRecognizer : public MultiDragGestureRecognizer {
  TDF_REFF_CLASS_META(ImmediateMultiDragGestureRecognizer)
 public:
  ImmediateMultiDragGestureRecognizer() = default;
  std::shared_ptr<MultiDragPointerState> CreateNewPointerState(const PointerData &data) const override;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::ImmediateMultiDragGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::ImmediateMultiDragGestureRecognizer)