//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/engine/render/text_object.h"

namespace tdfcore {

class ClipObject : public RenderObject {
TDF_REFF_CLASS_META(ClipObject)

TRect GetChildVisibleRect(const RenderObject &child, const TRect &rect) override;

 protected:
  ClipObject();

  virtual TRect GetClipBounds() = 0;

  virtual std::shared_ptr<Layer> Clip(PaintingContext &context, TPoint offset,
                                      PaintingContext::PaintingContextCallback painter) = 0;

  void OnPaint(PaintingContext &context, const TPoint &offset) override;
  void OnDebugPaint(PaintingContext &context, const TPoint &offset) override;

  virtual bool HitTestClip(const TPoint &position) const = 0;

  /**
   * @brief 碰撞测试，把当前position位置中下所有 HitTestTarget 添加到 HitTestResult 中去。
   *
   * @param position 碰撞的位置
   * @param transform 相对于根节点的转变
   * @return 碰撞结果集合
   */
  std::shared_ptr<HitTestResult> HitTest(const TPoint &position) override;

  TPaint debug_paint_;
  std::shared_ptr<TextObject> debug_text_object_;
  size_t debug_text_offset_y_ = 0;

  static constexpr TPoint kDebugTextOffsetFactor = {8.0, 1.2};
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::ClipObject, bases < tdfcore::RenderObject >)
TDF_REFL_END(tdfcore::RenderObject)
