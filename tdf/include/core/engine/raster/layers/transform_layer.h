//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <string>

#include "core/common/id_generator.h"
#include "core/common/m33.h"
#include "core/common/m44.h"
#include "core/engine/raster/layers/container_layer.h"
#include "core/engine/render/render_property.h"

namespace tdfcore {
class LayerTreeBuilder;

/**
 * @brief       矩阵变换layer
 */
class TransformLayer : public ContainerLayer {
  TDF_REFF_CLASS_META(TransformLayer)
 public:
  void AddToLayerTree(std::shared_ptr<LayerTreeBuilder> builder,
                      const TPoint &offset = TPoint::Make(0, 0)) override;

  void SetTransform(const TM44 &value);
  constexpr TM44 GetTransform() const { return transform_; }
  std::string ToString() const override;

  void ApplyTransform(const std::shared_ptr<Layer> &child, TM44 &transform) override;

  void Preroll(PrerollContext &context, const TM33 &matrix) override;

  void Paint(PaintContext &context) const override;

 protected:
  explicit TransformLayer(const TM44 &transform, uint64_t id = IDGenerator::NextUniqueID());

 private:
  TM44 transform_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::TransformLayer, bases<tdfcore::ContainerLayer>)
TDF_REFL_MEMBER(transform_);
TDF_REFL_END(tdfcore::TransformLayer)
