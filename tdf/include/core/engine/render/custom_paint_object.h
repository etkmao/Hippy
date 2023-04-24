//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/raster/layers/custom_paint_layer.h"
#include "core/engine/render/render_object.h"
#include "core/engine/render/render_property.h"
#include "custom_painter.h"

namespace tdfcore {

/**
 * @brief 自定义绘制
 * @details
 * 该类可自定义背景与前景的绘制效果，子节点位于背景与前景之间。通过CustomPainter来实现背景与前景的绘制操作。
 */
class CustomPaintObject : public RenderObject {
  TDF_REFF_CLASS_META(CustomPaintObject)

 public:
  std::shared_ptr<CustomPainter> GetPainter() const { return painter_; }

  void SetPainter(const std::shared_ptr<CustomPainter> &painter);

 protected:
  explicit CustomPaintObject(const std::shared_ptr<CustomPainter> &painter = nullptr);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

  bool AlwaysNeedsCompositing() override { return true; }

 private:
  std::shared_ptr<CustomPainter> painter_;
  std::shared_ptr<CustomPainterLayer> custom_painter_layer_;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::CustomPaintObject, bases<tdfcore::RenderObject>)
TDF_REFL_END(tdfcore::CustomPaintObject)
