//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/recognizer/drag_detail.h"
#include "core/support/gesture/recognizer/multi_drag_pointer_state.h"
#include "core/tdfi/timer.h"

namespace tdfcore {
class DelayedPointerState : public MultiDragPointerState,
                                           public std::enable_shared_from_this<DelayedPointerState> {
 public:
  ~DelayedPointerState() override = default;

  void Accepted(GestureMultiDragStartCallback starter) override;

  void Dispose() override;

 protected:
  DelayedPointerState(PointerID pointer, TPoint initial_position, TDuration delay = kLongPressTimeout);
  void CheckForResolutionAfterMove() override;
  void Init() override;

 private:
  void DelayPassed();
  void EnsureTimerStopped();

 private:
  std::shared_ptr<Timer> timer_;
  TDuration delay_ = TDuration::zero();
  GestureMultiDragStartCallback stater_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
