//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <string>

#include "core/engine/graphics/picture.h"
#include "core/engine/raster/layers/layer.h"

namespace tdfcore {
class LayerTreeBuilder;

/**
 * @brief       绘制SkPicture内容的叶子Layer
 */
class PictureLayer : public Layer {
  TDF_REFF_CLASS_META(PictureLayer)

 public:
  void AddToLayerTree(std::shared_ptr<LayerTreeBuilder> builder, const TPoint& offset = TPoint::Make(0, 0)) override;

  void SetPicture(const std::shared_ptr<Picture>& picture);

  /**
   * @brief     设置该图层是否为复杂图层，默认为false
   * @details   RasterCache会根据这个标记判断是否值得对picture进行光栅化缓存
   *            见RasterCache::IsPictureWorthRasterizing
   * @param     value
   */
  void SetIsComplex(bool value);

  constexpr bool IsComplex() const { return is_complex_; }

  /**
   * @brief     设置该图层是否易变，默认为false
   * @details   RasterCache会根据这个标记判断是否值得对picture进行光栅化缓存
   *            见`RasterCache::IsPictureWorthRasterizing`
   * @param     value
   */
  void SetWillChange(bool value);

  constexpr bool IsWillChange() const { return will_change_; }

  std::string ToString() const override;

  void Preroll(PrerollContext& context, const TM33& matrix) override;

  void Paint(PaintContext& context) const override;

  std::shared_ptr<Picture> GetPicture() const { return picture_; }

 protected:
  PictureLayer(const TPoint& offset = TPoint::Make(0, 0), bool is_complex = false, bool will_change = false,
               uint64_t id = IDGenerator::NextUniqueID());

 private:
  // Even though pictures themselves are not GPU resources, they may reference
  // images that have a reference to a GPU resource.
  std::shared_ptr<Picture> picture_;
  TPoint offset_;
  bool is_complex_;
  bool will_change_;
  bool is_out_of_bounds_ = false;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::PictureLayer, bases<tdfcore::Layer>)
TDF_REFL_MEMBER(offset_);
TDF_REFL_MEMBER(is_complex_);
TDF_REFL_MEMBER(will_change_);
TDF_REFL_END(tdfcore::PictureLayer)
