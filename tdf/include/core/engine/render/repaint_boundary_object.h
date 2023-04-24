//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include "core/engine/render/render_object.h"

namespace tdfcore {
/**
 * @brief 重绘边界修饰渲染对象
 */
class RepaintBoundaryObject : public RenderObject {
  TDF_REFF_CLASS_META(RepaintBoundaryObject)
 protected:
  explicit RepaintBoundaryObject(bool repaint_boundary = true);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::RepaintBoundaryObject, bases<tdfcore::RenderObject>)
  TDF_REFL_MEMBER(is_repaint_boundary_);
TDF_REFL_END(tdfcore::RepaintBoundaryObject)