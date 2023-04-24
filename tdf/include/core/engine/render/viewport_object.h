//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/render_object.h"

namespace tdfcore {

/**
 * @brief 特殊的可以挂载多个子节点的容器节点，显示的区域是实际大小的一个子集，例如列表页
 */
class ViewportObject : public RenderObject {
  TDF_REFF_CLASS_META(ViewportObject)
 protected:
  explicit ViewportObject(bool clip_to_bounds = true);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ViewportObject, bases<tdfcore::RenderObject>)
TDF_REFL_END(tdfcore::ViewportObject)