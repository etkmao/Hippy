//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/box_shadow.h"
#include "core/engine/render/render_property.h"
#include "core/engine/render/render_object.h"

namespace tdfcore {

/**
 * @brief 带阴影的矩形单子节点容器
 */
class ShadowObject : public RenderObject {
  TDF_REFF_CLASS_META(ShadowObject)

 public:
  /**
   * @brief 设置阴影
   * @param shadow 阴影
   */
  void SetShadow(const BoxShadow &shadow);

  /**
   * @brief 获取阴影
   */
  const BoxShadow &GetShadow() const;

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
  explicit ShadowObject(const BoxShadow &shadow);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  void PaintShadow(Canvas *canvas, const TRect &bounds);
  BoxShadow shadow_ = BoxShadow::None();
  std::array<TScalar, 8> radius_{};

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ShadowObject, bases<tdfcore::RenderObject>)
  TDF_REFL_MEMBER(shadow_);
TDF_REFL_END(tdfcore::ShadowObject)
