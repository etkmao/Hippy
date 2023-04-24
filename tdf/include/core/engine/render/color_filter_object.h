//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/render_property.h"
#include "core/engine/graphics/color_filter.h"
#include "core/engine/render/render_object.h"

namespace tdfcore {

/**
 * @brief 滤镜
 * @details 子节点将被应用滤镜效果
 */

class ColorFilterObject : public RenderObject {
  TDF_REFF_CLASS_META(ColorFilterObject)
 public:
  /**
   * @brief 设置滤镜
   * @param filter 滤镜
   */
  void SetFilter(const std::shared_ptr<ColorFilter> &filter);

 protected:
  explicit ColorFilterObject(const std::shared_ptr<ColorFilter> &filter = nullptr);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  std::shared_ptr<ColorFilter> color_filter_ = nullptr;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ColorFilterObject, bases<tdfcore::RenderObject>)
TDF_REFL_END(tdfcore::ColorFilterObject)
