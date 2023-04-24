//

//
// Copyright (c) Tencent Corporation. All rights reserved.
//
//

#pragma once

#include <vector>

#include "core/engine/raster/layers/transform_layer.h"
#include "core/engine/render/render_property.h"

namespace tdfcore {
class LayerTreeBuilder;
class LayerLink;
class FollowerLayer : public TransformLayer {
  TDF_REFF_CLASS_META(FollowerLayer)

 public:
  void AddToLayerTree(std::shared_ptr<LayerTreeBuilder> builder, const TPoint &offset) override;
  void ApplyTransform(const std::shared_ptr<Layer> &child, TM44 &transform) override;
  std::string ToString() const override;
  TM44 GetLastTransform() const;
  void SetLayerLink(std::shared_ptr<LayerLink> link);
  void SetLinkOffset(const TPoint &offset);
  void SetOffset(const TPoint &offset);
  void SetShowWhenUnlinked(bool is_show_when_unlinked);

 protected:
  explicit FollowerLayer(std::shared_ptr<LayerLink> link, const TPoint &link_offset = TPoint::Make(0, 0),
                         const TPoint &unlink_offset = TPoint::Make(0, 0), bool is_show_when_unlinked = true,
                         uint64_t id = IDGenerator::NextUniqueID());
  bool EstablishTransform();
  static TM44 CollectTransformForLayerChain(const std::vector<std::shared_ptr<ContainerLayer>> &layer_list);

 private:
  std::shared_ptr<LayerLink> layer_link_;
  TPoint link_offset_;
  TPoint unlink_offset_;
  TPoint last_offset_;
  bool is_show_when_unlinked_;
  bool is_establish_succeed_ = false;
  TM44 last_transform_;
  TM44 inverted_transform_;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::FollowerLayer, bases<tdfcore::TransformLayer>)
TDF_REFL_MEMBER(layer_link_);
TDF_REFL_MEMBER(unlink_offset_);
TDF_REFL_MEMBER(link_offset_);
TDF_REFL_MEMBER(is_show_when_unlinked_);
TDF_REFL_MEMBER(is_establish_succeed_);
TDF_REFL_MEMBER(last_offset_);
TDF_REFL_MEMBER(last_transform_);
TDF_REFL_MEMBER(inverted_transform_);
TDF_REFL_END(tdfcore::FollowerLayer)
