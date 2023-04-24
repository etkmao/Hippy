//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/event/pointer_data.h"
#include "core/support/gesture/recognizer/one_sequence_gesture_recognizer.h"

namespace tdfcore {

enum class ForceState {
  // 手指还没按下前，初始状态，准备接受手势
  kReady,
  // 手指按下，但还没有产生压力
  kPossible,
  // 手指按下，从竞技场中获胜；注意，即使获胜了，但是如果压力值没有到阈值，也不会产生回调
  kAccepted,
  // 手指按下，压力值超过startPressurce阈值
  kStarted,
  // 手指按下，压力值超过peakPressurce阈值
  kPeeked,
};

struct ForcePressDetails {
  double pressure_ = 0.0;
  /// 全局坐标系
  TPoint position{TPoint()};
};

using ForcePressStartCallback = std::function<void(const ForcePressDetails &force_press_details)>;
using ForcePressPeakCallback = std::function<void(const ForcePressDetails &force_press_details)>;
using ForcePressUpdateCallback = std::function<void(const ForcePressDetails &force_press_details)>;
using ForcePressEndCallback = std::function<void(const ForcePressDetails &force_press_details)>;

class ForcePressGestureRecognizer : public OneSequenceGestureRecognizer {
  TDF_REFF_CLASS_META(ForcePressGestureRecognizer)
 public:
  explicit ForcePressGestureRecognizer(double startPressure = 0.4, double peakPressure = 0.85)
      : startPressure_(startPressure), peakPressure_(peakPressure) {}

  /**
   * @brief 设置 start 回调
   * @param on_start 回调
   */
  void SetStart(const ForcePressStartCallback &on_start) { on_force_press_start_ = on_start; }

  /**
   * @brief 设置 peak 回调
   * @param on_peak 回调
   */
  void SetPeak(const ForcePressPeakCallback &on_peak) { on_force_press_peak_ = on_peak; }

  /**
   * @brief 设置 update 回调
   * @param on_update 回调
   */
  void SetUpdate(const ForcePressUpdateCallback &on_update) { on_force_press_update_ = on_update; }

  /**
   * @brief 设置 end 回调
   * @param on_end 回调
   */
  void SetEnd(const ForcePressEndCallback &on_end) { on_force_press_end_ = on_end; }

 protected:
  bool CanAddPointer(const PointerData &data) const override;
  void AcceptGesture(PointerID pointer) override;
  void RejectGesture(PointerID pointer) override;
  void DidStopTrackingLastPointer(PointerID pointer) override;
  void HandlePointerDown(const PointerData &data) override;
  void HandlePointerMove(const PointerData &data) override;
  void HandlePointerUp(const PointerData &data) override;
  void HandlePointerCancel(const PointerData &data) override;

 private:
  double startPressure_ = 0.0;
  double peakPressure_ = 0.0;
  double lastPressure_ = 0.0;
  TPoint last_position_;
  ForceState state_ = ForceState::kReady;

  ForcePressStartCallback on_force_press_start_;
  ForcePressPeakCallback on_force_press_peak_;
  ForcePressUpdateCallback on_force_press_update_;
  ForcePressEndCallback on_force_press_end_;

  static double Interpolation(double min, double max, double value);
  void HandleForceGestureState(const PointerData &data);
};

}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::ForcePressGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::ForcePressGestureRecognizer)