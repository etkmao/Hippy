//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/common/m44.h"
#include "core/engine/graphics/texture.h"

namespace tdfcore {
class SnapshotDelegate {
 public:
  virtual std::shared_ptr<Texture> MakeRasterSnapshot(std::shared_ptr<Picture> picture, TSize frame_size,
                                                      double pixel_ratio, TM44 matrix) = 0;
};
}  // namespace tdfcore
