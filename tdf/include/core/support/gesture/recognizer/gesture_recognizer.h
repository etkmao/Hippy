//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <unordered_set>
#include "core/support/gesture/arena/gesture_arena_member.h"
#include "core/support/gesture/event/pointer_data.h"

namespace tdfcore {
class PointerHandler;

class GestureRecognizer : public GestureArenaMember {
  TDF_REFF_CLASS_META(GestureRecognizer)
 public:

  /**
   * @brief 配置手势是否生效，默认为 true
   * @param is_enabled true 生效，false 不生效
   */
  constexpr void SetEnabled(bool is_enabled) { enabled_ = is_enabled; }

  /**
   * @brief 检测手势是否生效
   * @return true 生效，false 不生效
   */
  constexpr bool IsEnabled() const { return enabled_; }

  /**
   * 此手势是否可以处理这个 pointer data
   * @param data 指针数据
   */
  virtual bool CanAddPointer(const PointerData &data) const;

  /**
   * @brief 设置输入设备类型的遮罩
   * @param device_kind_mask 输入设备遮罩
   * 设置该mask之后，遮罩内的输入设备类型将会被该手势忽略
   */
  void SetDeviceKindMask(const std::unordered_set<DeviceKind> &device_kind_mask);

  /**
    @brief 获取输入设备类型遮罩
   */
  constexpr const std::unordered_set<DeviceKind> &GetDeviceKindMask() const { return device_kind_mask_; }

  /**
   * @brief 设置指针类型的遮罩
   * @param pointer_button_mask 指针类型遮罩
   * 设置该mask后，遮罩内的点击类型将会被手势忽略
   */
  void SetPointerButtonMask(const std::unordered_set<PointerButtonType>& pointer_button_mask);

  /**
   * @breif 获取指针类型遮罩
   */
  constexpr const std::unordered_set<PointerButtonType> &GetPointerButtonMask() const { return pointer_button_mask_; }

 private:
  bool enabled_ = true;
  std::unordered_set<DeviceKind> device_kind_mask_;
  std::unordered_set<PointerButtonType> pointer_button_mask_;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::GestureRecognizer, bases<>)
TDF_REFL_END(tdfcore::GestureRecognizer)