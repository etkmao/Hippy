//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/arena/gesture_arena_member.h"
#include "core/support/gesture/constants.h"
#include "core/support/gesture/recognizer/velocity_tracker.h"
#include "core/support/gesture/recognizer/one_sequence_gesture_recognizer.h"
#include "core/tdfi/timer.h"

namespace tdfcore {
struct LongPressStartDetails {
 public:
  explicit LongPressStartDetails(const TPoint &position) : position(position) {}
  /// 全局坐标系
  TPoint position = TPoint::Make(0, 0);
  PointerButtonType button_type = PointerButtonType::kPrimary;
  DeviceKind kind = DeviceKind::kTouch;
};

struct LongPressMoveUpdateDetails {
 public:
  LongPressMoveUpdateDetails(const TPoint &position, const TPoint &offset_from_origin)
      : position(position), offset_from_origin(offset_from_origin) {}

  TPoint position = TPoint::Make(0, 0);
  TPoint offset_from_origin = TPoint::Make(0, 0);
  PointerButtonType button_type = PointerButtonType::kPrimary;
  DeviceKind kind = DeviceKind::kTouch;
};

struct LongPressEndDetails {
 public:
  LongPressEndDetails(const TPoint &position, const Velocity &velocity) : position(position), velocity(velocity) {}

  TPoint position = TPoint::Make(0, 0);
  Velocity velocity;
  PointerButtonType button_type = PointerButtonType::kPrimary;
  DeviceKind kind = DeviceKind::kTouch;
};

using long_press_closure = std::function<void()>;
using long_press_start_closure = std::function<void(const LongPressStartDetails &long_press_start_details)>;
using long_press_move_update_closure =
    std::function<void(const LongPressMoveUpdateDetails &long_press_move_update_details)>;
using long_press_up_closure = std::function<void()>;
using long_press_end_closure = std::function<void(const LongPressEndDetails &long_press_end_details)>;

class LongPressGestureRecognizer : public OneSequenceGestureRecognizer {
  TDF_REFF_CLASS_META(LongPressGestureRecognizer)
 public:
  explicit LongPressGestureRecognizer(TDuration deadline = kLongPressTimeout) : deadline_(deadline) {}

  long_press_closure on_long_press_;
  long_press_start_closure on_long_press_start_;
  long_press_move_update_closure on_long_press_move_update_;
  long_press_up_closure on_long_press_up_;
  long_press_end_closure on_long_press_end_;

 protected:
  void HandlePointerDown(const PointerData &data) override;
  void HandlePointerMove(const PointerData &data) override;
  void HandlePointerUp(const PointerData &data) override;
  void HandlePointerCancel(const PointerData &data) override;

  bool CanAddPointer(const PointerData &data) const override;
  void AcceptGesture(PointerID pointer) override;
  void RejectGesture(PointerID pointer) override;
  void DidStopTrackingLastPointer(PointerID pointer) override;

 private:
  void CheckLongPressStart(const PointerData &data);
  void CheckLongPressMove(const PointerData &data);
  void CheckLongPressEnd(const PointerData &data);
  bool IsLegalPointer(PointerID pointer) const;
  void Reset();

  std::shared_ptr<VelocityTracker> velocity_tracker_;
  // 正在监视的手势事件 ID
  PointerID primary_pointer_ = 0;
  // 在一序列手势事件中，是否触发过 down event 事件
  bool is_down_ = false;
  // 手势是否进入到 Accept 状态
  bool is_accept_ = false;
  // 初始化时手势的位置
  TPoint initial_position_;
  // 计算按压时间的定时器
  std::shared_ptr<Timer> timer_;
  // 长按时间阈值
  TDuration deadline_;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::LongPressGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::LongPressGestureRecognizer)