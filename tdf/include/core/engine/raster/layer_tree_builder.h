//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <vector>
#include "core/engine/raster/layer_tree.h"
#include "core/common/rect.h"
#include "core/common/point.h"

namespace tdfcore {

class ContainerLayer;
class PictureLayer;
class EmbeddedLayer;
class Layer;
class LayerLink;

/**
 * @brief       从flushPaint后的Layer Tree构建出提交render的Layer Tree
 */
class LayerTreeBuilder : public Object {
 public:
  ~LayerTreeBuilder() override;

  std::unique_ptr<LayerTree> Build();

  void AddPicture(std::shared_ptr<PictureLayer> layer, const TPoint &offset);
  void AddPlatformView(std::shared_ptr<EmbeddedLayer> layer, const EmbeddedInfo& info);

  void AddLayer(std::shared_ptr<Layer> layer);
  void PushLayer(std::shared_ptr<ContainerLayer> layer);
  void PopLayer();

  const std::vector<std::shared_ptr<ContainerLayer>> &GetLayerStack() { return layer_stack_; }
  const std::vector<EmbeddedInfo> &GetEmbeddedInfo() { return embedded_views_info_; }

 protected:
  LayerTreeBuilder();

 private:
  std::vector<std::shared_ptr<ContainerLayer>> layer_stack_;
  std::vector<EmbeddedInfo> embedded_views_info_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
