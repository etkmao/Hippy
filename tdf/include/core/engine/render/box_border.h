//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/common/color.h"
#include "core/common/property.h"
#include "core/common/rect.h"
#include "core/common/rrect.h"
#include "core/engine/raster/painting_context.h"

namespace tdfcore {

static inline bool HasValue(const std::array<TScalar, 8> &s) {
#define NZ(x, y) (!TScalarNearlyZero(x) && !TScalarNearlyZero(y))
  return NZ(s[0], s[1]) || NZ(s[2], s[3]) || NZ(s[4], s[5]) || NZ(s[6], s[7]);
#undef NZ
}

static inline RRect MakeRRect(const TRect &rect, const std::array<TScalar, 8> &s) {
  RRect rrect;
  const std::array<TPoint, 4> corners = {{{s[0], s[1]}, {s[2], s[3]}, {s[4], s[5]}, {s[6], s[7]}}};
  rrect.SetRectRadii(rect, corners);
  return rrect;
}

static inline std::array<TScalar, 8> ASSERT_AND_COPY(const std::array<TScalar, 8> &radii) {
  std::array<TScalar, 8> radius{};
#define ASSERT_AND_ASSIGN(idx)                               \
  assert(std::isfinite(radii[idx]) && (radii[idx] >= 0.0f)); \
  radius[idx] = radii[idx]

  ASSERT_AND_ASSIGN(0);
  ASSERT_AND_ASSIGN(1);
  ASSERT_AND_ASSIGN(2);
  ASSERT_AND_ASSIGN(3);
  ASSERT_AND_ASSIGN(4);
  ASSERT_AND_ASSIGN(5);
  ASSERT_AND_ASSIGN(6);
  ASSERT_AND_ASSIGN(7);

#undef ASSERT_AND_ASSIGN
  return radius;
}

/**
 * @brief 装饰性边框的形状
 */
enum class BoxShape {
  /// 矩形
  kRectangle,

  /// 圆形
  kCircle
};

/**
 * @brief 装饰性边框的边线的线型
 */
enum class BorderLineStyle {
  /// 无任何绘制效果
  kNone,

  /// 实线
  kSolid
};

/**
 * @brief 装饰性边框的边线
 */
class BorderStyle {
 public:
  /**
   * @brief 构造函数
   * @param color 颜色
   * @param width 线宽
   * @param style 线型
   */
  BorderStyle(Color color, float width, BorderLineStyle style = BorderLineStyle::kSolid);

  /**
   * @brief 构造一条无任何装饰效果的边线
   */
  static BorderStyle None() { return {Color::Transparent(), 0.0, BorderLineStyle::kNone}; }

  /**
   * @brief 颜色
   */
  constexpr Color GetColor() const { return color_; }

  /**
   * @brief 线宽
   */
  constexpr TScalar Width() const { return width_; }

  /**
   * @brief 线型
   */
  constexpr BorderLineStyle Style() const { return style_; }

  /**
   * @brief 生成一个SkPaint对象，用于描述边线的绘制效果
   */
  TPaint ToPaint() const;

  /**
   * @brief 边线是否无任何装饰效果
   */
  constexpr bool IsNone() const {
    return (color_.GetA() == 0) || TScalarNearlyZero(width_) || (style_ == BorderLineStyle::kNone);
  }

  std::vector<std::shared_ptr<Property>> MakeDiagnosticsProperties() const;

 private:
  Color color_ = Color::Transparent();
  TScalar width_ = kNearlyZero;
  BorderLineStyle style_ = BorderLineStyle::kNone;

  friend bool operator==(const BorderStyle &a, const BorderStyle &b);
  friend bool operator!=(const BorderStyle &a, const BorderStyle &b);
};

/**
 * @brief 装饰性边框
 */
class BoxBorder {
 public:
  /**
   * @brief 构造函数
   * @param left 左侧边线
   * @param top 顶部边线
   * @param right 右侧边线
   * @param bottom 底部边线
   */
  BoxBorder(BorderStyle left, BorderStyle top, BorderStyle right, BorderStyle bottom)
      : left_(left), top_(top), right_(right), bottom_(bottom) {}

  /**
   * @brief 构造一个无任何装饰效果的边框实例
   */
  static BoxBorder None() { return BoxBorder::All(BorderStyle::None()); }

  /**
   * @brief 构造一个边框，其四条边线的效果完全相同
   * @param side 参考边线
   */
  static BoxBorder All(const BorderStyle &style) { return {style, style, style, style}; }

  /**
   * @brief 绘制操作
   * @param context 绘制的上下文环境
   * @param bounds 边框的位置与尺寸
   * @param shape 边框形状
   * @param radius 边框的圆角尺寸
   */
  void Paint(Canvas *canvas, const TRect &bounds, const BoxShape &shape, const std::array<TScalar, 8> &radii);

  /**
   * @brief 边框是否无任何装饰效果
   */
  constexpr bool IsNone() const {
    return left_.IsNone() && top_.IsNone() && right_.IsNone() && bottom_.IsNone();
  }

  /**
   * @brief 边框的四条边线的效果是否完全相同
   */
  bool IsUniform() const;

  std::vector<std::shared_ptr<Property>> MakeDiagnosticsProperties() const;

 private:
  BorderStyle left_;
  BorderStyle top_;
  BorderStyle right_;
  BorderStyle bottom_;

  void PaintBorders(Canvas *canvas, const TRect &bounds);
  static void PaintUniformBorder(Canvas *canvas, const TRect &bounds, BoxShape shape, const BorderStyle &side,
                                 const std::array<TScalar, 8> &radii);

  friend bool operator==(const BoxBorder &a, const BoxBorder &b);
  friend bool operator!=(const BoxBorder &a, const BoxBorder &b);
};

}  // namespace tdfcore
