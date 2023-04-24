//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/recognizer/multi_drag_pointer_state.h"
#include "core/support/gesture/recognizer/drag_detail.h"

namespace tdfcore {

class VerticalPointerState : public MultiDragPointerState {
 public:
  virtual ~VerticalPointerState() = default;

  void Accepted(GestureMultiDragStartCallback starter) override;

 protected:
  VerticalPointerState(PointerID pointer, TPoint initial_position)
      : MultiDragPointerState(pointer, initial_position) {}
  void CheckForResolutionAfterMove() override;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
