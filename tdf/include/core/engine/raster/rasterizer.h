//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <optional>
#include "core/engine/graphics/board.h"
#include "core/engine/raster/compositor_context.h"
#include "core/engine/raster/layer_tree.h"
#include "core/engine/raster/snapshot_delegate.h"
#include "core/platform/common/embedded_view_manager.h"

namespace tdfcore {
/**
 * @brief       Rasterizer持有当前on-screen surface，负责将LayerTree光栅化并上屏。
 *              CompositorContext是Rasterizer的上下文结构，其中保存了上屏所必须的GPU状态信息。
 */
class Rasterizer : public Object, public SnapshotDelegate {
 public:
  ~Rasterizer() override = default;

  /**
   * @brief     渲染LayerTree
   */
  void Draw(std::unique_ptr<LayerTree> layer_tree, const std::shared_ptr<Board> &surface);

  /**
   * @brief     获取当前CompositorContext，永远不会为nullptr。
   */
  RasterContext *GetCompositorContext() { return &compositor_context_; }

  /**
   * @brief     将截图的Picture光栅化
   */
  std::shared_ptr<Texture> MakeRasterSnapshot(std::shared_ptr<Picture> picture, TSize frame_size,
                                              double pixel_ratio = 1.0, TM44 matrix = TM44()) override;
  /**
   * @brief 获取最新的LayerTree截屏Bitmap
   * @param pixel_ratio  缩放系数
   */
  std::shared_ptr<Texture> TakeLastLayerTreeScreenshot(double pixel_ratio = 1.0);

  void SetDevice(const std::shared_ptr<Device> &device) { device_ = device; }

 protected:
  explicit Rasterizer(const std::shared_ptr<Device> &resource_device);

 private:
  std::shared_ptr<Device> device_;
  std::shared_ptr<Device> resource_device_;
  RasterContext compositor_context_;
  std::unique_ptr<LayerTree> last_layer_tree_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
