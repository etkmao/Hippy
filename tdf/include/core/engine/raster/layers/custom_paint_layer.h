//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <string>

#include "core/engine/raster/layers/container_layer.h"
#include "core/engine/render/custom_painter.h"

namespace tdfcore {

class LayerTreeBuilder;

/**
 * @brief       颜色滤镜layer
 */
class CustomPainterLayer : public Layer {
  TDF_REFF_CLASS_META(CustomPainterLayer)

 public:
  void AddToLayerTree(std::shared_ptr<LayerTreeBuilder> builder, const TPoint &offset) override;
  std::string ToString() const override;

  void Preroll(PrerollContext &context, const TM33 &matrix) override;

  void Paint(PaintContext &context) const override;

  /**
   * @brief     设置CustomPainter
   * @param     custom_painter CustomPainter
   */
  void SetCustomPainter(const std::shared_ptr<CustomPainter> &custom_painter);

  /**
   * @brief     获取CustomPainter
   * @return    CustomPainter
   */
  std::shared_ptr<CustomPainter> GetCustomPainter();

  void SetOffset(const TPoint &offset) { offset_ = offset; }

  const TPoint &GetOffset() const { return offset_; }

  void SetPicture(const std::shared_ptr<Picture> &picture) { picture_ = picture; }

  std::shared_ptr<Picture> GetPicture() { return picture_; }

 protected:
  explicit CustomPainterLayer(const std::shared_ptr<CustomPainter> &custom_painter,
                              uint64_t id = IDGenerator::NextUniqueID());

 private:
  TPoint offset_;
  std::shared_ptr<CustomPainter> custom_painter_;
  std::shared_ptr<Picture> picture_;
  RasterCacheResult raster_cache_result_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::CustomPainterLayer, bases<tdfcore::Layer>)
TDF_REFL_MEMBER(custom_painter_);
TDF_REFL_END(tdfcore::CustomPainterLayer)
