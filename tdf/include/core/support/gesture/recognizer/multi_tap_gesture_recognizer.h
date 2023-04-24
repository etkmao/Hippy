//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/recognizer/gesture_recognizer.h"
#include "core/tdfi/timer.h"

namespace tdfcore {
struct MultiTapDetails {
  // 全局坐标系位置
  TPoint position = TPoint::Make(0, 0);

  explicit MultiTapDetails(const TPoint&position) : position(position) {}
};

using multi_tap_down_closure = std::function<void(PointerID, MultiTapDetails)>;
using multi_tap_up_closure = std::function<void(PointerID, MultiTapDetails)>;
using multi_tap_closure = std::function<void(PointerID)>;
class MultiTapGestureRecognizer;

/**
 * TapGesture 是 MultiTapGestureRecognizer 的一部分，表示由单个点击序列产生的完整手势
 * TapGesture 不是 GestureRecognizer, 不会去抢占竞技场
 */
class TapGesture : public Object, public std::enable_shared_from_this<TapGesture> {
 public:
  virtual ~TapGesture() = default;
  TapGesture(const std::shared_ptr<MultiTapGestureRecognizer>& multi_tap_gesture_recognizer,
             const PointerData &data, TDuration long_tap_delay);

 private:
  TPoint last_position_;
  std::optional<TPoint> final_position_;
  TPoint initial_global_position_;
  std::shared_ptr<Timer> timer_;
  bool win_arena_ = false;
  PointerID pointer_;
  std::weak_ptr<MultiTapGestureRecognizer> multi_tap_gesture_recognizer_;

  void Accept();
  void Check();
  void HandlePointerDown(const PointerData &data);
  void HandlePointerUp(const PointerData &data);
  void HandlePointerCancel(const PointerData &data);
  void Reject();
  void Cancel();
  void StopTrackingPointer();
  void StopTimer();

  std::shared_ptr<MultiTapGestureRecognizer> GetRecognizer() { return multi_tap_gesture_recognizer_.lock(); }

  friend MultiTapGestureRecognizer;
};

/**
 * 识别多个点击，每个点击都是独立的，比如说
 * down-1, down-2, up-1, up-2
 * 上面产生的两个点击分别是 up-1, up-2
 */
class MultiTapGestureRecognizer : public GestureRecognizer {
  TDF_REFF_CLASS_META(MultiTapGestureRecognizer)
 public:
  explicit MultiTapGestureRecognizer(TDuration long_tap_delay = TDuration(0));

  multi_tap_down_closure on_tap_down_;
  multi_tap_up_closure on_tap_up_;
  multi_tap_closure on_tap_;
  multi_tap_closure on_tap_cancel_;
  multi_tap_down_closure on_long_tap_down_;

 protected:
  void AcceptGesture(PointerID pointer) override;
  void RejectGesture(PointerID pointer) override;
  bool CanAddPointer(const PointerData &data) const override;
  void HandlePointerDown(const PointerData &data) override;
  void HandlePointerMove(const PointerData &data) override;
  void HandlePointerUp(const PointerData &data) override;
  void HandlePointerCancel(const PointerData &data) override;

 private:
  std::map<PointerID, std::shared_ptr<TapGesture>> gestures_;
  TDuration long_tap_delay_;

  void DispatchTap(PointerID pointer, const TPoint& position);
  void DispatchCancel(PointerID pointer);
  void DispatchLongTap(PointerID pointer, const TPoint& position) const;
  void RemovePointer(PointerID pointer);
  bool CheckPointerIsValid(PointerID pointer);

  friend TapGesture;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::MultiTapGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::MultiTapGestureRecognizer)