//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/recognizer/multi_drag_pointer_state.h"
#include "core/support/gesture/recognizer/drag_detail.h"

namespace tdfcore {
class HorizontalPointerState : public MultiDragPointerState {
 public:
  ~HorizontalPointerState() override = default;
  void Accepted(GestureMultiDragStartCallback starter) override;

 protected:
  HorizontalPointerState(PointerID pointer, TPoint initial_position)
      : MultiDragPointerState(pointer, initial_position) {}
  void CheckForResolutionAfterMove() override;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore