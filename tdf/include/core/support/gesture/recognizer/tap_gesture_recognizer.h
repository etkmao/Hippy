//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <string>

#include "core/support/gesture/event/pointer_data.h"
#include "core/support/gesture/recognizer/one_sequence_gesture_recognizer.h"
#include "core/support/gesture/constants.h"
#include "core/tdfi/timer.h"

namespace tdfcore {
struct TapDetails {
  // 全局坐标系位置
  TPoint position = TPoint::Make(0, 0);
  // 主键和其他按钮的区分
  PointerButtonType button_type = PointerButtonType::kPrimary;
  // 修饰键
  ModifierFlags modifier_flags = 0;

  TapDetails(const TPoint &position, PointerButtonType button_type, ModifierFlags modifier_flags)
    : position(position), button_type(button_type), modifier_flags(modifier_flags) {}

  explicit TapDetails(const TPoint &position) : position(position) {}
};

using TapDownClosure = std::function<void(const TapDetails &details)>;
using TapClosure = std::function<void()>;
using TapUpClosure = std::function<void(const TapDetails &details)>;

class TapGestureRecognizer : public OneSequenceGestureRecognizer {
  TDF_REFF_CLASS_META(TapGestureRecognizer)
 public:
  /**
   * 设置 tap down 回调
   * @param on_tap_down 回调
   */
  void SetTapDown(const TapDownClosure &on_tap_down) { on_tap_down_ = on_tap_down; }

  /**
   * 设置 tap up 回调
   * @param on_tap_up 回调
   */
  void SetTapUp(const TapUpClosure &on_tap_up) { on_tap_up_ = on_tap_up; }

  /**
   * 设置 tap 回调
   * @param on_tap 回调
   */
  void SetTap(const TapClosure &on_tap) { on_tap_ = on_tap; }

  /**
   * 设置 tap cancel 回调
   * @param on_tap_cancel 回调
   */
  void SetTapCancel(const TapClosure &on_tap_cancel) { on_tap_cancel_ = on_tap_cancel; }

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
  // 是否触发过 down 的回调
  bool sent_tap_down_ = false;
  // 是否接收过 down event
  bool is_down_ = false;
  // 正在监听的手势事件
  PointerID monitor_pointer_;

  TPoint initial_position_;
  TPoint up_position_;
  std::shared_ptr<Timer> timer_;
  // 鼠标左右键的区分
  PointerButtonType button_type_ = PointerButtonType::kPrimary;
  ModifierFlags modifier_flags_ = ModifierFlag::kUnknow;

  TapDownClosure on_tap_down_;
  TapUpClosure on_tap_up_;
  TapClosure on_tap_;
  TapClosure on_tap_cancel_;

  void CheckDown();
  void CheckUp();
  void CheckCancel();
  bool IsLegalPointer(PointerID pointer) const;
  void StopTimer();
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::TapGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::TapGestureRecognizer)