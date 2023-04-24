//
// Copyright (c) 2020 Tencent. All rights reserved.

//

#pragma once

#include <map>
#include <memory>
#include "core/support/gesture/constants.h"
#include "core/support/gesture/event/pointer_data.h"
#include "core/support/gesture/pointer_handler.h"
#include "core/support/gesture/recognizer/gesture_recognizer.h"
#include "core/tdfi/timer.h"

namespace tdfcore {
struct DoubleTapDetails {
  TPoint first_tap_position;
  TPoint second_tap_position;
  PointerButtonType button_type = PointerButtonType::kPrimary;
  DeviceKind kind = DeviceKind::kTouch;

  DoubleTapDetails(const TPoint &first, const TPoint &second) : first_tap_position(first),
  second_tap_position(second) {}
};

using DoubleTapClosure = std::function<void(const DoubleTapDetails &details)>;

/**
 * @brief 点击事件跟踪，根据点击距离和时间，确认是否要继续跟踪这个事件
 */
class TapTracker : public Object, public std::enable_shared_from_this<TapTracker> {
 public:
  ~TapTracker() override;

 protected:
  explicit TapTracker(const PointerData &data);
  void Init() override;

 private:
  bool IsWithinGlobalTolerance(const PointerData &data, double tolerance);
  bool HasElapsedMinTime() const;

  TPoint initial_global_position_;
  bool timeout_ = false;
  PointerID pointer_;

  std::shared_ptr<Timer> timer_;

  friend class DoubleTapGestureRecognizer;

  FRIEND_OF_TDF_ALLOC;
};

class DoubleTapGestureRecognizer : public GestureRecognizer {
  TDF_REFF_CLASS_META(DoubleTapGestureRecognizer)
 public:

  /**
   * 设置 double tap 的回调
   * @param on_double_tap 回调
   */
  void SetDoubleTap(const DoubleTapClosure &on_double_tap) { on_double_tap_ = on_double_tap; }

 protected:
  // 手势生命周期
  void AcceptGesture(PointerID pointer) override;
  void RejectGesture(PointerID pointer) override;
  bool CanAddPointer(const PointerData &data) const override;
  void HandlePointerDown(const PointerData &data) override;
  void HandlePointerMove(const PointerData &data) override;
  void HandlePointerUp(const PointerData &data) override;
  void HandlePointerCancel(const PointerData &data) override;

 private:
  void TrackTap(const PointerData &data);
  void Reject(const std::shared_ptr<TapTracker>& tracker);
  void Reset();
  void ClearTrackers();
  void StartDoubleTapTimer();
  void StopDoubleTapTimer();
  void CheckUp(const DoubleTapDetails &details);
  std::shared_ptr<TapTracker> GetTracker(const PointerData &data);

  std::shared_ptr<TapTracker> current_tracker_tap_;
  std::map<PointerID, std::shared_ptr<TapTracker>> trackers_;
  std::shared_ptr<Timer> double_tap_timer_;
  DoubleTapClosure on_double_tap_;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::DoubleTapGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::DoubleTapGestureRecognizer)