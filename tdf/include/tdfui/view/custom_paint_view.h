//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/engine/render/custom_paint_object.h"
#include "tdfui/view/view.h"

namespace tdfcore {

/**
 * @brief 自定义绘制View
 */
class CustomPaintView : public View {
 public:
  void SetPainter(const std::shared_ptr<CustomPainter> &painter);

  std::shared_ptr<CustomPainter> GetPainter() const;

  void MarkNeedsPaint();

 protected:
  explicit CustomPaintView(const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent())
      : View(view_context) {}
  void Init() override;
  enum RenderObjectIndex { kCustomPaint = View::kRenderObjectCount, kRenderObjectCount };
  int32_t GetTotalRenderObjectCount() override;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
