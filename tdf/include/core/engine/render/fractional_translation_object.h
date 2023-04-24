//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/point.h"
#include "core/engine/render/render_object.h"

namespace tdfcore {
/**
 * @brief 按比例平移对象
 * 在实际Paint的时候，会把当前平移系数*bounds宽高得到实际的平移参数
 */
class FractionalTranslationObject : public RenderObject {
  TDF_REFF_CLASS_META(FractionalTranslationObject)

 public:
  /**
   * @brief 设置平移系数
   * @param translation 平移系数
   */
  void SetTranslation(TPoint translation);

  void ApplyPaintTransform(std::shared_ptr<RenderObject> child, TM33 &transform) const override;
  /**
  * @brief 碰撞测试，把当前position位置中下所有 HitTestTarget 添加到 HitTestResult 中去。
  *
  * @param position 碰撞的位置
  * @param transform 相对于根节点的转变
  * @return 碰撞的结果集合
  */
  std::shared_ptr<HitTestResult> HitTest(const TPoint &position) override;

 protected:
  explicit FractionalTranslationObject(TPoint translation);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  TPoint translation_ = TPoint::Make(0, 0);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::FractionalTranslationObject, bases<tdfcore::RenderObject>)
  TDF_REFL_MEMBER(translation_);
TDF_REFL_END(tdfcore::FractionalTranslationObject)