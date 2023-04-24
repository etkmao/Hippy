//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include "core/engine/graphics/canvas.h"
#include "core/common/object.h"

namespace tdfcore {

class NWayCanvas : public Canvas {
 public:
  virtual void AddCanvas(Canvas *canvas) = 0;
  static std::shared_ptr<NWayCanvas> Make(int width, int height);
};

}  // namespace tdfcore
