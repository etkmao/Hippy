//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/recognizer/one_sequence_gesture_recognizer.h"
#include "core/support/gesture/event/pointer_data.h"

namespace tdfcore {

class EagerGestureRecognizer : public OneSequenceGestureRecognizer {
  TDF_REFF_CLASS_META(EagerGestureRecognizer)
 public:
  EagerGestureRecognizer() = default;

 protected:
  void HandlePointerDown(const PointerData& data) override;
  void AcceptGesture(PointerID pointer) override;
  void RejectGesture(PointerID pointer) override;
  void DidStopTrackingLastPointer(PointerID pointer) override;
};

}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::EagerGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::EagerGestureRecognizer)