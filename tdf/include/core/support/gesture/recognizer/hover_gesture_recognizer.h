//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/recognizer/one_sequence_gesture_recognizer.h"

namespace tdfcore {
struct HoverDetails {
  /// 全局坐标系位置
  TPoint position = TPoint::Make(0, 0);

  /// 修饰键
  ModifierFlags modifier_flags = 0;
};

class HoverGestureRecognizer : public OneSequenceGestureRecognizer {
  TDF_REFF_CLASS_META(HoverGestureRecognizer)
 public:
  bool CanAddPointer(const PointerData &data) const override;

  /**
   * @brief 设置进入区域时的回调
   */
  void SetEnter(const std::function<void(HoverDetails)> &on_entering_function);

  /**
   * @brief 获取进入区域时的回调
   */
  constexpr const std::function<void(HoverDetails)> &GetEnter() const { return on_entering_function_; }

  /**
   * @brief 设置在区域内更新时的回调
   */
  void SetUpdate(const std::function<void(HoverDetails)> &on_updating_function);

  /**
   * @brief 获取在区域内更新时的回调
   */
  constexpr const std::function<void(HoverDetails)> &GetUpdate() const { return on_updating_function_; }

  /**
   * @brief 设置离开区域时的回调
   */
  void SetLeave(const std::function<void(HoverDetails)> &on_leaving_function);

  /**
   * @brief 获取离开区域时的回调
   */
  constexpr const std::function<void(HoverDetails)> &GetLeave() const { return on_leaving_function_; }

 protected:
  void HandleEventHover(const PointerData &data) override;
  void AcceptGesture(PointerID pointer) override;
  void RejectGesture(PointerID pointer) override;
  void DidStopTrackingLastPointer(PointerID pointer) override;

 private:
  bool is_entering_;
  std::function<void(HoverDetails)> on_entering_function_;
  std::function<void(HoverDetails)> on_updating_function_;
  std::function<void(HoverDetails)> on_leaving_function_;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::HoverGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::HoverGestureRecognizer)