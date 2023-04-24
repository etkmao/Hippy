//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "core/common/object.h"
#include "core/engine/raster/compositor_context.h"
#include "core/engine/raster/layers/layer.h"
#include "core/platform/common/viewport_metrics.h"

namespace tdfcore {
class NWayCanvas;
class EmbeddedLayer;
/**
 * @brief       PlatformView相关信息
 */
struct EmbeddedInfo {
  TRect rect;
  uint64_t id;
  bool has_overlay;
  std::string view_type;
  std::map<std::string, std::string> property;
  std::weak_ptr<EmbeddedLayer> layer;

  bool operator==(const EmbeddedInfo &info) const {
    return rect == info.rect && id == info.id && has_overlay == info.has_overlay && view_type == info.view_type &&
           property == info.property;
  }
};

/**
 * @brief       维护一颗LayerTree
 */
class LayerTree : public Object {
 public:
  ~LayerTree() override;

  std::vector<EmbeddedInfo> GetEmbeddedInfo() const;

  /**
   * @brief     根据绘制顺序遍历一次LayerTree，用于计算渲染阶段依赖的信息
   * @param     frame 即将渲染的帧
   * @param     ignore_raster_cache 是否不进行raster cache
   *
   * @return    返回渲染目标十分需要readback
   */
  bool Preroll(RasterContext::RasterFrame &frame, bool ignore_raster_cache = false);

  /**
   * @brief     渲染LayerTree
   * @param     frame 渲染目标
   * @param     ignore_raster_cache 是否忽略raster cache
   */
  void Paint(RasterContext::RasterFrame &frame, bool ignore_raster_cache = false) const;

  /**
   * @brief     将LayerTree在指定区域中的内容绘制到SkPicture中
   * @param     bounds 指定区域
   * @return
   */
  std::shared_ptr<Picture> Flatten(const TRect &bounds);

  /**
   * @brief     获取根图层
   * @return    根图层指针
   */
  std::shared_ptr<Layer> GetRootLayer() const { return root_layer_; }

  /**
   * @brief     设置根图层
   * @param     root_layer 根图层指针
   */
  void SetRootLayer(std::shared_ptr<Layer> root_layer) { root_layer_ = std::move(root_layer); }

  void SetViewportMetrics(const ViewportMetrics &metrics) { viewport_metrics_ = metrics; }

  constexpr const ViewportMetrics &GetViewportMetrics() const { return viewport_metrics_; }

 protected:
  LayerTree(const std::shared_ptr<Layer> &root_layer, const std::vector<EmbeddedInfo> &info);

 private:
  std::shared_ptr<Layer> root_layer_;
  std::vector<EmbeddedInfo> embedded_info_;
  bool is_checkerboard_raster_cache_images_enabled_ = false;
  bool is_checkerboard_offscreen_layers_enabled_ = false;
  ViewportMetrics viewport_metrics_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
