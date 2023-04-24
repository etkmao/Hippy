//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/recognizer/multi_drag_gesture_recognizer.h"

namespace tdfcore {
/**
 * @brief 监听水平方向手势滑动事件，配合[HorizontalPointerState]使用，当水平滑动距离超过kTouchSlop时竞技成功
 */
class HorizontalMultiDragGestureRecognizer : public MultiDragGestureRecognizer {
  TDF_REFF_CLASS_META(HorizontalMultiDragGestureRecognizer)
 public:
  HorizontalMultiDragGestureRecognizer() = default;
  std::shared_ptr<MultiDragPointerState> CreateNewPointerState(const PointerData &data) const override;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::HorizontalMultiDragGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::HorizontalMultiDragGestureRecognizer)