//

//
// Copyright (c) Tencent Corporation. All rights reserved.
//
//

#pragma once

#include "core/engine/raster/layers/container_layer.h"
#include "core/engine/raster/layers/layer_link.h"
#include "core/engine/raster/layers/transform_layer.h"
#include "core/engine/render/render_property.h"

namespace tdfcore {
class LayerTreeBuilder;

class LeaderLayer : public TransformLayer {
  TDF_REFF_CLASS_META(LeaderLayer)

 public:
  std::shared_ptr<LayerLink> GetLink() const { return link_; }

  void SetLink(const std::shared_ptr<LayerLink> &link) { link_ = link; }

  constexpr TPoint GetOffset() const { return offset_; }

  void SetOffset(const TPoint &offset) { offset_ = offset; }

  void Attach(std::shared_ptr<RenderObject> child) override;

  void Detach() override;

  void AddToLayerTree(std::shared_ptr<LayerTreeBuilder> builder, const TPoint &offset) override;

  void ApplyTransform(const std::shared_ptr<Layer> &child, TM44 &transform) override;

  std::string ToString() const override;

 protected:
  explicit LeaderLayer(std::shared_ptr<LayerLink> link, const TPoint &offset = TPoint::Make(0, 0),
                       uint64_t id = IDGenerator::NextUniqueID());

 private:
  std::shared_ptr<LayerLink> link_;

  TPoint offset_;

  TPoint last_offset_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::LeaderLayer, bases<tdfcore::TransformLayer>)
TDF_REFL_MEMBER(link_);
TDF_REFL_MEMBER(offset_);
TDF_REFL_MEMBER(last_offset_);
TDF_REFL_END(tdfcore::LeaderLayer)
