//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include "core/common/m33.h"
#include "core/common/object.h"
#include "core/engine/graphics/board.h"
#include "core/engine/raster/raster_cache.h"
#include "core/platform/common/embedded_view_manager.h"

namespace tdfcore {

class LayerTree;

/**
 * @brief       合成上下文，每个Rasterizer一个
 */
class RasterContext {
 public:
  /**
   * @brief     ScopedFrame表示一次上屏流程的上下文，是一个逻辑结构
   */
  class RasterFrame {
   public:
    RasterContext &context;
    std::shared_ptr<Device> device;
    std::shared_ptr<Device> resource_device;
    Canvas *canvas = nullptr;
    std::shared_ptr<Board> board;
    const TM33 &root_transform;
  };

  RasterCache raster_cache;
  size_t frame_count = 0;
};

}  // namespace tdfcore
