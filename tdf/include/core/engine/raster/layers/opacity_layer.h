//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <string>

#include "core/engine/raster/layers/container_layer.h"
#include "core/engine/render/render_property.h"

namespace tdfcore {

class LayerTreeBuilder;

/**
 * @brief       alpha layer
 * @attention   绘制OpacityLayer需要调用saveLayer，这是一个开销非常大的方法，
 *              不要将没有子layer的OpacityLayer加入layer tree,
 *              `Preroll`中会进行是否具有子layer的断言检查。
 */
class OpacityLayer : public ContainerLayer {
  TDF_REFF_CLASS_META(OpacityLayer)

 public:
  void AddToLayerTree(std::shared_ptr<LayerTreeBuilder> builder,
                      const TPoint& offset = TPoint::Make(0, 0)) override;
  std::string ToString() const override;

  /**
   * @brief     设置alpha值
   * @param alpha
   */
  void SetAlpha(Alpha alpha);

  void Append(std::shared_ptr<Layer> child) override;

  void Preroll(PrerollContext& context, const TM33& matrix) override;

  void Paint(PaintContext& context) const override;

 protected:
  explicit OpacityLayer(Alpha alpha = 0, const TPoint& offset = TPoint::Make(0, 0),
                        uint64_t id = IDGenerator::NextUniqueID());

 private:
  void SetChildContainer(std::shared_ptr<ContainerLayer> layer);
  std::shared_ptr<ContainerLayer> GetChildContainer() const;
  Alpha alpha_;
  TPoint offset_;
  std::shared_ptr<ContainerLayer> child_container_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::OpacityLayer, bases<tdfcore::ContainerLayer>)
TDF_REFL_MEMBER(alpha_);
TDF_REFL_MEMBER(offset_);
TDF_REFL_END(tdfcore::OpacityLayer)
