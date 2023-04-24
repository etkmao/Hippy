//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/raster/layers/container_layer.h"
#include "core/engine/render/render_property.h"

namespace tdfcore {
/**
 * @brief       裁剪Layer基类
 */
class ClipLayer : public ContainerLayer {
  TDF_REFF_CLASS_META(ClipLayer)

 public:
  void AddToLayerTree(std::shared_ptr<LayerTreeBuilder> builder, const TPoint &offset) override;
  /**
   * @brief     设置是否反锯齿
   * @param     anti_alias true/false，开启/关闭反锯齿
   */
  void SetAntiAlias(bool is_anti_alias) { is_anti_alias_ = is_anti_alias; }

  /**
   * @brief     获取是否反锯齿
   * @return    是否反锯齿
   */
  bool GetAntiAlias() const { return is_anti_alias_; }

  void Paint(PaintContext &context) const override;
  virtual bool IsUsesSaveLayer() const { return false; }

 protected:
  explicit ClipLayer(bool is_anti_alias = true, uint64_t id = IDGenerator::NextUniqueID())
      : ContainerLayer(id), is_anti_alias_(is_anti_alias) {}

  virtual void DoClipToLayerTree(std::shared_ptr<LayerTreeBuilder> builder) const = 0;

  /**
   * @brief     执行具体的裁剪操作，由子类实现
   */
  virtual void DoClipPaint(Canvas *canvas) const = 0;

  bool is_anti_alias_;
  TRect save_layer_bounds_ = TRect::MakeEmpty();

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ClipLayer, bases<tdfcore::ContainerLayer>)
TDF_REFL_MEMBER(is_anti_alias_);
TDF_REFL_MEMBER(save_layer_bounds_);
TDF_REFL_END(tdfcore::ClipLayer)
