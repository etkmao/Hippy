//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/color.h"
#include "core/common/point.h"
#include "core/common/property.h"
#include "core/engine/graphics/paint.h"

namespace tdfcore {
/**
 * @brief 边框阴影
 */
class BoxShadow {
 public:
  /**
   * @brief 构造函数
   * @param color 阴影颜色
   * @param offset 阴影偏移
   * @param blur_radius 模糊半径
   * @param spread_radius 扩散半径
   */
  explicit BoxShadow(Color color = Color::Transparent(), TPoint offset = TPoint::Make(0, 0), float blur_radius = 0,
                     float spread_radius = 0);

  /**
   * @brief 生成一个TPaint对象，用于描述阴影的绘制效果
   */
  TPaint ToPaint() const;

  /**
   * @brief 构造一个无任何阴影效果实例
   */
  static BoxShadow None() { return BoxShadow{}; }

  /**
   * @brief 颜色
   */
  constexpr Color GetColor() const { return color_; }

  /**
   * @brief 偏移
   */
  constexpr TPoint Offset() const { return offset_; }

  /**
   * @brief 模糊半径
   */
  constexpr float BlurRadius() const { return blur_radius_; }

  /**
   * @brief 扩散半径
   */
  constexpr float SpreadRadius() const { return spread_radius_; }

  std::vector<std::shared_ptr<Property>> MakeDiagnosticsProperties() const;

 private:
  Color color_;
  TPoint offset_;
  float blur_radius_;
  float spread_radius_;

  friend bool operator==(const BoxShadow &a, const BoxShadow &b);
  friend bool operator!=(const BoxShadow &a, const BoxShadow &b);

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
