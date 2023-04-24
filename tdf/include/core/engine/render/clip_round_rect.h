//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/clip_object.h"
#include "core/common/rrect.h"
#include "core/engine/render/render_property.h"
namespace tdfcore {

/**
 * @brief 圆角矩形剪裁
 * @details 子节点将被应用圆角矩形剪裁效果
 */

class ClipRRectObject : public ClipObject {
TDF_REFF_CLASS_META(ClipRRectObject)

 public:
/**
 * @brief 设置圆角矩形剪裁区域
 * @param rect 圆角矩形
 */
  void SetClipRRect(const RRect &rrect);

 protected:
  explicit ClipRRectObject(const RRect &rrect = RRect::MakeEmpty());

  TRect GetClipBounds() override;
  std::shared_ptr<Layer> Clip(PaintingContext &context, TPoint offset,
                              PaintingContext::PaintingContextCallback painter) override;

  bool HitTestClip(const TPoint &position) const override;

  void OnDebugPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  RRect rrect_ = RRect::MakeEmpty();

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ClipRRectObject, bases < tdfcore::ClipObject >)
  TDF_REFL_MEMBER(rrect_);
TDF_REFL_END(tdfcore::ClipRRectObject)
