//
// Copyright (c) 2020 Tencent. All rights reserved.

//

#pragma once

#include "core/support/gesture/recognizer/scroll_gesture_recognizer.h"

namespace tdfcore {
class PanGestureRecognizer : public ScrollGestureRecognizer {
  TDF_REFF_CLASS_META(PanGestureRecognizer)
 protected:
  bool IsFlingGesture(const VelocityEstimate &estimate) const override;
  TPoint GetDelta(const TPoint &delta) const override;
  double GetPrimaryValue(const TPoint &value) const override;

  PanGestureRecognizer() {
    min_global_moved_distance_ = kPanSlop;
  }

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::PanGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::PanGestureRecognizer)