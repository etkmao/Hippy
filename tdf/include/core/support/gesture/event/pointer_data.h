//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/m44.h"
#include "core/common/serialization.h"
#include "core/common/time.h"
#include "core/platform/common/keyboard/keyboard_constants.h"

namespace tdfcore {

using PointerID = int64_t;

enum class PointerType : int64_t {
  kUnsupported = 1,  // 未支持的点击事件
  kDown = 2,
  kMove = 3,
  kUp = 4,
  kStationary = 5,
  kCancel = 7,
};

enum class DeviceKind : int64_t {
  /// 手指
  kTouch,
  /// 鼠标
  kMouse,
  /// 触控笔
  kStylus,
};

enum class SignalKind : int64_t {
  /// 普通信号
  kNormal,
  /// 滑动信号
  kScroll,
  /// 放大缩小信号
  kScale,
  /// 旋转信号
  kRoate,
  /// 悬浮信号
  kHover
};

/// 点击按钮类型
enum class PointerButtonType : int64_t {
  /// 鼠标移动，没有任何按钮或者手指触发
  kNone,
  /// 鼠标左键，手指Touch
  kPrimary,
  /// 鼠标右键
  kSecondary,
  // 鼠标中键/滚轮
  kMiddle,
};

struct PointerData {
  PointerType type = PointerType::kUnsupported;
  PointerButtonType button_type = PointerButtonType::kPrimary;
  DeviceKind device_kind = DeviceKind::kTouch;
  SignalKind signal_kind = SignalKind::kNormal;
  TDuration time_stamp = TDuration::zero();
  PointerID pointer = 0;
  /// 全局坐标系
  TPoint position = TPoint::Make(0, 0);
  TPoint delta = TPoint::Make(0, 0);
  ModifierFlags modifier_flags = ModifierFlag::kUnknow;
  int64_t device = 0;
  double pressure = 1.0;
  double pressure_min = 1.0;
  double pressure_max = 1.0;
  double scale = 0.0;

  TDF_SERIALIZATION(TDF_SERIALIZABLE_ENUM(type, PointerType, int64_t),
                    TDF_SERIALIZABLE_ENUM(button_type, PointerButtonType, int64_t),
                    TDF_SERIALIZABLE_ENUM(device_kind, DeviceKind, int64_t),
                    TDF_SERIALIZABLE_ENUM(signal_kind, SignalKind, int64_t), time_stamp, pointer, position, delta,
                    modifier_flags, device, pressure, pressure_min, scale);

  /**
   * @brief 对position进行transform坐标变换
   * @return 返回变换后的位置点
   */
  static TPoint TransformPosition(const std::shared_ptr<TM44> &transform, const TPoint &position);

  /**
   * @brief 对untransformed_delta（moveEvent移动后的位置）做transform坐标变换
   * @return 返回变换后的位置点
   */
  static TPoint TransformDeltaViaPositions(const std::shared_ptr<TM44> &transform,
                                           const TPoint &untransformed_end_position, const TPoint &untransformed_delta,
                                           const TPoint &transformed_end_position = TPoint::Make(-100, 0));
};

}  // namespace tdfcore
