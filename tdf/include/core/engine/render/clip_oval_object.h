//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/engine/render/clip_object.h"
#include "core/engine/render/render_property.h"
namespace tdfcore {

/**
 * @brief 椭圆形剪裁
 * @details 子节点将被应用椭圆形剪裁效果
 */
class ClipOvalObject : public ClipObject {
  TDF_REFF_CLASS_META(ClipOvalObject)

 public:
  /**
   * @brief 通过指定的外切矩形来设置椭圆形剪裁区域
   * @param rect 外切矩形
   */
  void SetRect(const TRect &rect);

 protected:
  explicit ClipOvalObject(const TRect &rect);

  TRect GetClipBounds() override;
  std::shared_ptr<Layer> Clip(PaintingContext &context, TPoint offset,
                              PaintingContext::PaintingContextCallback painter) override;

  bool HitTestClip(const TPoint &position) const override;

  void OnDebugPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  TRect rect_ = TRect::MakeEmpty();

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ClipOvalObject, bases<tdfcore::ClipObject>)
TDF_REFL_MEMBER(rect_);
TDF_REFL_END(tdfcore::ClipOvalObject)