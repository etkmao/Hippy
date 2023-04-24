// Copyright (c) 2020 Tencent. All rights reserved.

//
#pragma once

#include "core/support/gesture/recognizer/scroll_gesture_recognizer.h"

namespace tdfcore {

class HorizontalPanGestureRecognizer : public ScrollGestureRecognizer {
  TDF_REFF_CLASS_META(HorizontalPanGestureRecognizer)
 protected:
  bool IsFlingGesture(const VelocityEstimate &estimate) const override;
  TPoint GetDelta(const TPoint &delta) const override;
  double GetPrimaryValue(const TPoint &value) const override;
  void HandlePointerMove(const PointerData &data) override;

  void Init() override;

 protected:
  HorizontalPanGestureRecognizer() = default;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::HorizontalPanGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::HorizontalPanGestureRecognizer)