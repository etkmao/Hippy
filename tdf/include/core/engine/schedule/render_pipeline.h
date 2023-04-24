//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <thread>

#include "core/tdfi/event_center.h"
#include "core/engine/raster/layer_tree.h"
#include "core/engine/raster/rasterizer.h"
#include "core/engine/render/repaint_boundary_object.h"
#include "core/engine/render/render_tree.h"
#include "core/platform/common/output/output.h"
#include "core/support/gesture/recognizer/gesture_recognizer.h"
#include "core/common/spsc_queue.h"
#include "core/engine/graphics/picture.h"
#include "core/engine/graphics/device.h"
#include "core/engine/schedule/pipeline_metric.h"
#include "core/engine/schedule/scheduler.h"

namespace tdfcore {

/**
 * @brief Pipeline数据
 */
class PipelineData : public Object {
 public:
  ~PipelineData() override = default;
  std::shared_ptr<RenderTree> render_tree;
  std::shared_ptr<Picture> picture;
  std::unique_ptr<LayerTree> layer_tree;
  FrameTiming timing;
  int64_t id = 0;
  explicit PipelineData(const int64_t data_id) : id(data_id) {}
};

typedef std::unique_ptr<PipelineData> PipelineDataPtr;

constexpr uint8_t kMaxPipelineDataNumber = 3;
static constexpr const char *kRenderPipelineBuildEventLabel = "BuildRenderTree";
static constexpr const char *kRenderPipelinePaintEventLabel = "Paint";
static constexpr const char *kRenderPipelineRasterEventLabel = "Raster";

enum class RenderPipelineMode {
  /// 普通模式
  /// 不会同步等待创建Embedded的过程
  kNormal,

  /// 截图模式
  /// 创建Emebedded与截图Embedded都会同步等待
  kSnapshot
};

class EmbeddedViewManager;
class PlatformView;
/**
 * @brief 动态化框架内核渲染调度器
 */
class RenderPipeline : public Object, public std::enable_shared_from_this<RenderPipeline> {
 public:
  ~RenderPipeline() override = default;

  /**
   * @brief 标记需要构建RenderObject树
   * Pipeline将在下一个Vsync来临时重新构建RenderObject树
   */
  void SetNeedsBuild();

  /**
   * @brief 设置`构建RenderObject树`的实现
   * Pipeline判断如果需要`构建RenderObject树`，将会调用此实现
   * @param func `构建RenderObject树`的实现
   */
  void SetBuildFunction(std::function<std::shared_ptr<RenderTree>()> func);

  /**
   * @brief 设置动态化框架输出
   */
  void SetOutput(std::shared_ptr<Output> output);

  /**
   * @brief 标记开启弃帧功能
   */
  void SetFrameDiscardEnabled(bool frame_discard_enabled) { frame_discard_enabled_ = frame_discard_enabled; }

  /**
   * @brief 获取是否开启弃帧功能
   */
  constexpr const bool GetFrameDiscardEnabled() const { return frame_discard_enabled_; }

  /**
   * @brief 执行一次完整的Pipeline流程
   * 一般由Vsync信号触发，将按顺序判断是否需要执行(Build、Paint、Raster)
   */
  void RunOnce();

  /**
   * @brief 标记需要重新Paint
   * Pipeline会在下次vsync从根节点执行Paint操作
   */
  void MarkNeedsPaint();

  /**
   * @brief 获取上一次RenderTree的根节点
   * @return RenderTree根节点
   */
  constexpr const std::weak_ptr<RenderTree> &GetLastRenderTree() const { return last_render_tree_; }

  /**
   * @brief     收集帧信息
   * @attention warning：现在没有地方设置，也没有地方使用，也没有地方管理，待删除or完善。
   */
  constexpr const FrameTimings &GetFrameTimings() const { return frame_timings_; }

  /**
   * @brief     最后一次帧耗时信息
   */
  constexpr const FrameTiming &GetLastFrameTiming() const { return last_frame_timing_; }

  /**
   * @brief 获取最新的截屏Bitmap，必须在GPUTaskRunner执行
   * @param down_scale  缩小系数
   */
  std::shared_ptr<Texture> TakeLastScreenshot(double pixel_ratio);

  const std::unique_ptr<Rasterizer> &GetRasterizer() { return rasterizer_; }

 protected:
  explicit RenderPipeline(const std::shared_ptr<Scheduler> &scheduler,
                          const std::shared_ptr<EngineClock> &clock,
                          const std::shared_ptr<EventCenter> &event_center,
                          const std::shared_ptr<Device> &resource_device,
                          const std::shared_ptr<RenderContext> &render_context = TDF_MAKE_SHARED(RenderContext),
                          RenderPipelineMode mode = RenderPipelineMode::kNormal);
  void Init() override;

 private:
  bool frame_discard_enabled_ = true;
  std::atomic<bool> is_building_ = false;
  std::shared_ptr<Output> output_;
  std::shared_ptr<Scheduler> scheduler_;
  std::shared_ptr<EngineClock> clock_;
  std::shared_ptr<EventCenter> event_center_;
  std::shared_ptr<RenderContext> render_context_;
  spsc_queue<PipelineDataPtr> pipeline_data_queue_;
  PipelineDataPtr Build();
  void Paint(PipelineDataPtr data);
  void Raster(PipelineDataPtr data);
  void ScheduleBuildAndPaint();
  void ScheduleCombinedRaster();
  void ScheduleOneByOneRaster(PipelineDataPtr data);
  std::unique_ptr<LayerTree> CompositeFrame(std::shared_ptr<RenderObject> root);
  void MakeEmbeddedSnapshot(const PipelineDataPtr& data);

  std::atomic<bool> needs_build_ = true;

  std::function<std::shared_ptr<RenderTree>()> build_func_;
  std::weak_ptr<RenderTree> last_render_tree_;
  std::unique_ptr<Rasterizer> rasterizer_;
  static constexpr size_t max_frame_timings_ = 60 * 60;
  FrameTimings frame_timings_;
  FrameTiming last_frame_timing_;
  int64_t pipeline_data_id_ = 0;
  RenderPipelineMode mode_;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
