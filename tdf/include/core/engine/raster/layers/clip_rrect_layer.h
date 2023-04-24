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
 * @brief       裁剪圆角矩形区域
 */
class ClipRRectLayer : public ClipLayer {
  TDF_REFF_CLASS_META(ClipRRectLayer)

 public:
  std::string ToString() const override;

  /**
   * @brief     设置裁剪区域
   * @param     clip_rrect 圆角矩形区域
   */
  void SetClipRRect(RRect clip_rrect);

  /**
   * @brief     获取裁剪区域
   * @return    圆角矩形区域
   */
  RRect GetClipRRect();
  void Preroll(PrerollContext& context, const TM33& matrix) override;

 protected:
  explicit ClipRRectLayer(RRect clip_rrect, bool is_anti_alias = true, uint64_t id = IDGenerator::NextUniqueID())
      : ClipLayer(is_anti_alias, id), clip_rrect_(clip_rrect) {}

  void DoClipPaint(Canvas *canvas) const override;

  void DoClipToLayerTree(std::shared_ptr<LayerTreeBuilder> builder) const override;

 private:
  RRect clip_rrect_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ClipRRectLayer, bases<tdfcore::ClipLayer>)
TDF_REFL_MEMBER(clip_rrect_);
TDF_REFL_END(tdfcore::ClipRRectLayer)
