//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/clip_object.h"
#include "core/engine/render/render_property.h"

namespace tdfcore {

/**
 * @brief 矩形剪裁
 * @details 子节点将被应用矩形剪裁效果
 */

class ClipRectObject : public ClipObject {
  TDF_REFF_CLASS_META(ClipRectObject)

 public:
  /**
   * @brief 设置矩形剪裁区域
   * @param rect 矩形
   */
  void SetClipRect(const TRect &rect);

  /**
  * @brief 获取矩形剪裁区域
  * @return 剪裁区域
  */
  TRect GetClipRect() const { return rect_; }

 protected:
  explicit ClipRectObject(const TRect &rect);

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

TDF_REFL_DEFINE(tdfcore::ClipRectObject, bases<tdfcore::ClipObject>)
TDF_REFL_MEMBER(rect_);
TDF_REFL_END(tdfcore::ClipRectObject)
