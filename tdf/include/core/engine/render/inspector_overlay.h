//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <vector>
#include "core/engine/render/render_object.h"

namespace tdfcore {

/**
 * @brief 检查视图浮层绘制对象
 */
class RenderInspectorOverlay : public RenderObject {
TDF_REFF_CLASS_META(RenderInspectorOverlay)

 public:
  /**
   * @brief 获取候选绘制对象
   */
  const std::vector<std::weak_ptr<RenderObject>> &GetCandidates() const { return candidates_; }

  /**
   * @brief 更新候选绘制对象
   * @param 候选绘制对象 vector
   */
  void SetCandidates(std::vector<std::weak_ptr<RenderObject>> candidates);

 protected:
  explicit RenderInspectorOverlay(std::vector<std::weak_ptr<RenderObject>> candidates);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

  bool AlwaysNeedsCompositing() override { return true; }

 private:
  std::vector<std::weak_ptr<RenderObject>> candidates_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::RenderInspectorOverlay, bases<tdfcore::RenderObject>)
TDF_REFL_END(tdfcore::RenderInspectorOverlay)