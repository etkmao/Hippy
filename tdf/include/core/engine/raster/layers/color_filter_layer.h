//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <string>

#include "core/engine/raster/layers/container_layer.h"
#include "core/engine/graphics/color_filter.h"

namespace tdfcore {

class LayerTreeBuilder;

/**
 * @brief       颜色滤镜layer
 */
class ColorFilterLayer : public ContainerLayer {
  TDF_REFF_CLASS_META(ColorFilterLayer)

 public:
  void AddToLayerTree(std::shared_ptr<LayerTreeBuilder> builder, const TPoint& offset) override;
  std::string ToString() const override;

  void Preroll(PrerollContext& context, const TM33& matrix) override;

  void Paint(PaintContext& context) const override;

  /**
   * @brief     设置颜色滤镜
   * @param     color_filter 颜色滤镜
   */
  void SetColorFilter(const std::shared_ptr<ColorFilter>& color_filter);

  /**
   * @brief     获取颜色滤镜
   * @return    颜色滤镜
   */
  std::shared_ptr<ColorFilter> GetColorFilter();

 protected:
  explicit ColorFilterLayer(const std::shared_ptr<ColorFilter>& color_filter,
                            uint64_t id = IDGenerator::NextUniqueID());

 private:
  std::shared_ptr<ColorFilter> color_filter_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::ColorFilterLayer, bases<tdfcore::ContainerLayer>)
TDF_REFL_MEMBER(color_filter_);
TDF_REFL_END(tdfcore::ColorFilterLayer)
