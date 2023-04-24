//
//
// Copyright (c) Tencent Corporation. All rights reserved.
//
//
#pragma once

#include <memory>
#include <string>

#include "core/common/id_generator.h"
#include "core/common/m33.h"
#include "core/common/tree_node.h"
#include "core/engine/graphics/board.h"
#include "core/engine/raster/debug_info.h"
#include "core/engine/raster/raster_cache.h"
#include "core/engine/render/render_property.h"
#include "core/platform/common/view/mutators_stack.h"

namespace tdfcore {
class ContainerLayer;
class LayerTreeBuilder;
class RenderObject;

static constexpr TRect kGiantRect = TRect::MakeLTRB(-1E9F, -1E9F, 1E9F, 1E9F);

struct PrerollContext {
  RasterCache *raster_cache;
  std::shared_ptr<Device> device;
  std::shared_ptr<Device> resource_device;
  MutatorsStack &mutators_stack;
  std::shared_ptr<Board> board;
  TRect cull_rect;
  bool is_surface_needs_readback;
  const bool is_checkerboard_offscreen_layers_enabled;
  bool has_platform_view = false;
  bool is_opaque = true;
  RasterCachePolicy raster_cache_policy = RasterCachePolicy::kStrict;
  size_t total_picture_count = 0;
  size_t out_of_bounds_count = 0;
};

/**
 * @brief   逻辑及绘制操作的图层基类
 *
 * @details Layer具备真正的绘制逻辑。
 *          painting结束后，生成LayerTree，LayerTreeBuilder将这个LayerTree拷贝重建一份提交至render阶段。
 *          在render阶段，Rasterizer会根据LayerTree渲染画面。
 *
 * @note    实际上拷贝重建后LayerTree并不一定与原来RenderObject
 *          painting阶段生成的LayerTree一一对应。
 *          每个Layer拷贝过程中都有机会决定对其节点以及其子节点进行处理，例如FollowerLayer根据逻辑对子节点决定是否进行裁剪。
 *          最后导致提交至render阶段的LayerTree与原来的不一致。
 */
class Layer : public TreeNode<Layer, RenderObject> {
  TDF_REFF_CLASS_META(Layer)

 public:
  ~Layer() override;
  class AutoPrerollSaveLayerState {
   public:
    static AutoPrerollSaveLayerState Create(PrerollContext &preroll_context, bool is_save_layer_active = true,
                                            bool is_layer_itself_performs_readback = false);

    ~AutoPrerollSaveLayerState();

   private:
    AutoPrerollSaveLayerState(PrerollContext &preroll_context, bool is_save_layer_active,
                              bool is_layer_itself_performs_readback);

    PrerollContext &preroll_context_;
    bool is_save_layer_active_;
    bool is_layer_itself_performs_readback_;
    bool prev_surface_needs_readback_;
  };

  /**
   * @brief   LayerTree的绘制上下文
   */
  struct PaintContext {
    Canvas *internal_nodes_canvas;
    Canvas *leaf_nodes_canvas;
    std::unordered_map<int64_t, Canvas *> overlay_canvas;
    const RasterCache *raster_cache;
    const bool checkerboard_offscreen_layers;
    size_t total_paint_count;
    size_t paint_cache_hit_count;
    RasterCachePolicy raster_cache_policy = RasterCachePolicy::kStrict;
  };
  class AutoSaveLayer {
   public:
    static AutoSaveLayer Create(const PaintContext &paint_context, const TRect &bounds, const TPaint *paint);

    static AutoSaveLayer Create(const PaintContext &paint_context, const SaveLayerRec &layer_rec);

    ~AutoSaveLayer();

   private:
    AutoSaveLayer(const PaintContext &paint_context, const TRect &bounds, const TPaint *paint);

    AutoSaveLayer(const PaintContext &paint_context, const SaveLayerRec &layer_rec);

    const PaintContext &paint_context_;
    const TRect bounds_;
  };

  /**
   * @brief     根据绘制顺序遍历一次Layer节点，不同的Layer子类会进行其必要绘制前处理
   * @param     context preroll上下文
   * @param     matrix  当前图层的preroll矩阵
   */
  virtual void Preroll(PrerollContext &context, const TM33 &matrix) {}

  /**
   * @brief     绘制图层，由Layer子类实现
   * @param     context 绘制上下文
   */
  virtual void Paint(PaintContext &context) const = 0;

  /**
   * @brief     返回当前layer在Preroll阶段确定的绘制边界（参照本地坐标系）
   */
  constexpr const TRect &GetBounds() const { return bounds_; }

  /**
   * @brief     绘制边界必须在Preroll()中确定，否则Paint()中不会绘制任何内容
   * @param     bounds 边界
   */
  void SetBounds(const TRect &bounds) { bounds_ = bounds; }

  bool NeedsPaint() const { return !bounds_.IsEmpty(); }

  /**
   * @brief 将当前Layer加入LayerTree中
   * @param builder 当前LayerTreeBuilder
   * @param offset  目前无用，都是(0, 0)
   */
  virtual void AddToLayerTree(std::shared_ptr<LayerTreeBuilder> builder, const TPoint &offset = TPoint::Make(0, 0)) = 0;

  /**
   * @brief 从parent移除当前Layer
   */
  void Remove();

  /**
   * @brief     获取父Layer
   * @return parent layer
   */
  std::shared_ptr<ContainerLayer> GetParent() const;

  std::string ToString() const override;

  /**
   * @brief 设置纹理缓冲策略，影响范围为当前节点以及所有子节点
   */
  void SetRasterCachePolicy(RasterCachePolicy policy) { raster_cache_policy_ = policy; }

  /**
   * @brief 获取纹理缓冲策略
   */
  RasterCachePolicy GetRasterCachePolicy() const { return raster_cache_policy_; }

  /**
   * @brief 对象的调试分类信息
   */
  const char *GetDebugCategory() const override { return kLayerDebugCategory; }

 protected:
  explicit Layer(uint64_t id = IDGenerator::NextUniqueID());

  RasterCachePolicy raster_cache_policy_ = RasterCachePolicy::kInherited;

 private:
  TRect bounds_ = TRect::MakeEmpty();

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::Layer, bases<>)
TDF_REFL_MEMBER(id_);
TDF_REFL_MEMBER(bounds_);
TDF_REFL_END(tdfcore::Layer)
