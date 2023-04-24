//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/recognizer/scroll_gesture_recognizer.h"

namespace tdfcore {

class VerticalPanGestureRecognizer : public ScrollGestureRecognizer {
  TDF_REFF_CLASS_META(VerticalPanGestureRecognizer)
 protected:
  bool IsFlingGesture(const VelocityEstimate& estimate) const override;
  TPoint GetDelta(const TPoint& delta) const override;
  double GetPrimaryValue(const TPoint& value) const override;
  void HandlePointerMove(const PointerData &data) override;

  void Init() override;

 protected:
  VerticalPanGestureRecognizer() = default;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::VerticalPanGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::VerticalPanGestureRecognizer)