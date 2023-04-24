//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/gradient.h"
#include "core/engine/render/render_property.h"
#include "core/engine/render/render_object.h"
namespace tdfcore {

/**
 * @brief 带背景色的矩形单子节点容器
 */
class ColorObject : public RenderObject {
  TDF_REFF_CLASS_META(ColorObject)

 public:
  /**
   * @brief 设置颜色
   * @param color 颜色
   */
  void SetColor(Color color);

  /**
   * @brief 获取颜色
   */
  Color GetColor() const;

  /**
   * @brief 设置渐变色, gradient优先于color, 即如果为nullptr，则使用color
   * @param gradient 渐变
   */
  void SetGradient(const std::shared_ptr<Gradient> &gradient);

  /**
   * @brief 获取渐变背景色
   */
  const std::shared_ptr<Gradient> &GetGradient();

 protected:
  explicit ColorObject(Color color);

  explicit ColorObject(const std::shared_ptr<Gradient> &gradient);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  void PaintBackground(Canvas *canvas, const TRect &bounds);
  Color color_ = Color::Transparent();
  std::shared_ptr<Gradient> gradient_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ColorObject, bases<tdfcore::RenderObject>)
TDF_REFL_MEMBER(color_);
TDF_REFL_END(tdfcore::ColorObject)
