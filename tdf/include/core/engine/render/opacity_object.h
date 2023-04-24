//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/render_object.h"

namespace tdfcore {
/**
 * @brief 透明的修饰渲染对象
 */
class OpacityObject : public RenderObject {
  TDF_REFF_CLASS_META(OpacityObject)

 public:
  /**
   * @brief 设置不透明度
   * @param opacity 不透明度
   */
  void SetOpacity(TScalar opacity);

  /**
   * @brief 返回不透明度
   */
  TScalar GetOpacity() const;

 protected:
  explicit OpacityObject(TScalar opacity = 1.0f);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

  bool AlwaysNeedsCompositing() override;

 private:
  TScalar opacity_ = 1.0f;
  uint8_t alpha_ = 255;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::OpacityObject, bases<tdfcore::RenderObject>)
  TDF_REFL_MEMBER(opacity_);
  TDF_REFL_MEMBER(alpha_);
TDF_REFL_END(tdfcore::OpacityObject)
