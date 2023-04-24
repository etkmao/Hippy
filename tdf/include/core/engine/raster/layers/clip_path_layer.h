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
 * @brief       路径裁剪layer
 */

class ClipPathLayer : public ClipLayer {
  TDF_REFF_CLASS_META(ClipPathLayer)

 public:
  std::string ToString() const override;

  void Preroll(PrerollContext& context, const TM33& matrix) override;

  /**
   * @brief     设置裁剪路径
   * @param     clip_path 裁剪路径
   */
  void SetClipPath(const Path& clip_path);

  /**
   * @brief     获取裁剪路径
   * @return     裁剪路径
   */
  Path GetClipPath() const;

 protected:
  explicit ClipPathLayer(const Path& clip_path, bool anti_alias = true, uint64_t id = IDGenerator::NextUniqueID())
      : ClipLayer(anti_alias, id), clip_path_(clip_path) {}

  void DoClipToLayerTree(std::shared_ptr<LayerTreeBuilder> builder) const override;

  void DoClipPaint(Canvas *canvas) const override;

 private:
  Path clip_path_;

  FRIEND_OF_TDF_ALLOC;
};

template <>
class TypeProperty<Path> : public Property {
  TYPE_PROPERTY_INSTANTIATION(Path)
  std::string ValueToString() const override {
    /// TODO(kloudwang):可以根据具体需求输出要想的格式
    return "this is Path";
  }
};

}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::ClipPathLayer, bases<tdfcore::ClipLayer>)
TDF_REFL_END(tdfcore::ClipPathLayer)
