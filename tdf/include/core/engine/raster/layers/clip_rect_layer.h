//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <string>

#include "core/engine/raster/layers/clip_layer.h"
#include "core/engine/render/render_property.h"

namespace tdfcore {

class LayerTreeBuilder;

/**
 * @brief       矩形裁剪layer
 */
class ClipRectLayer : public ClipLayer {
  TDF_REFF_CLASS_META(ClipRectLayer)

 public:
  std::string ToString() const override;

  void Preroll(PrerollContext& context, const TM33& matrix) override;

  /**
   * @brief     设置裁剪区域
   * @param     clip_rect 裁剪区域
   */
  void SetClipRect(TRect clip_rect);

  /**
   * @brief     获取裁剪区域
   * @return    裁剪区域
   */
  TRect GetClipRect();

 protected:
  explicit ClipRectLayer(TRect clip_rect, bool is_anti_alias = true, uint64_t id = IDGenerator::NextUniqueID())
      : ClipLayer(is_anti_alias, id), clip_rect_(clip_rect) {}

  void DoClipPaint(Canvas *canvas) const override;

  void DoClipToLayerTree(std::shared_ptr<LayerTreeBuilder> builder) const override;

 private:
  TRect clip_rect_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ClipRectLayer, bases<tdfcore::ClipLayer>)
TDF_REFL_MEMBER(clip_rect_);
TDF_REFL_END(tdfcore::ClipRectLayer)
