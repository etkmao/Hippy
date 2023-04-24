//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/box_border.h"
#include "core/engine/render/render_property.h"
#include "core/engine/render/render_object.h"
namespace tdfcore {

/**
 * @brief 带边框的矩形单子节点容器
 */
class BorderObject : public RenderObject {
  TDF_REFF_CLASS_META(BorderObject)

 public:
  /**
   * @brief 设置边框
   * @param border 边框
   */
  void SetBorder(const BoxBorder &border);

  /**
   * @brief 获取边框
   */
  const BoxBorder &GetBorder() const;

  /**
   * @brief 设置圆角尺寸
   * @param radii 圆角尺寸, 分别对应4个角的x、y，共计8个值
   */
  void SetRadius(const std::array<TScalar, 8> &radii);

  /**
   * @brief 获取圆角尺寸
   */
  const std::array<TScalar, 8> &GetRadius() const;

 protected:
  explicit BorderObject(const BoxBorder &border);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  BoxBorder border_ = BoxBorder::None();
  std::array<TScalar, 8> radius_{};

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::BorderObject, bases<tdfcore::RenderObject>)
TDF_REFL_MEMBER(border_);
TDF_REFL_END(tdfcore::BorderObject)
