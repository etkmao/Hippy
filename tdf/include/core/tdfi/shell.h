//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <list>
#include "core/common/object.h"
#include "core/engine/graphics/device.h"
#include "core/engine/render/render_tree.h"
#include "core/engine/schedule/render_pipeline.h"
#include "core/engine/schedule/scheduler.h"
#include "core/platform/common/output/output.h"
#include "core/platform/common/platform.h"
#include "core/platform/common/text_input.h"
#include "core/platform/common/view/platform_view.h"
#include "core/support/profile/profiler.h"
#include "core/tdfi/event_center.h"
#include "core/tdfi/shell_config.h"
#include "core/tdfi/shortcut_manager.h"

namespace tdfcore {
/**
 * @brief 动态化框架 内核的接口层，内部将各个模块串联在一起对外提供接口进行服务
 */
class Shell : public Object, public std::enable_shared_from_this<Shell> {
 public:
  /**
   * @brief 创建Shell
   * @param platform 平台接口
   */
  static std::shared_ptr<Shell> Make(const std::shared_ptr<Platform> &platform, const ShellConfig &config);

  Shell(const Shell &shell) = delete;

  ~Shell() override;

  /**
   * @brief 返回当前线程的shell对象
   * @return shell对象的指针
   */
  static std::shared_ptr<Shell> GetCurrent();

  /**
   * @brief 获取平台接口
   * @return 返回Platform
   */
  std::shared_ptr<Platform> GetPlatform() const;

  /**
   * @brief 获取资源设备
   * @return resource_device
   */
  std::shared_ptr<Device> GetResourceDevice() const;

  /**
   * @brief 开始运行
   */
  void Start();

  /**
   * @brief 终止运行
   * @note 请在Shell析构之前主动调用本方法，调用之后，本实例将失效，不可再继续使用
   */
  void Terminate();

  /**
   * @brief 暂停运行
   * @note 调用Resume方法可恢复至运行状态
   */
  void Pause();

  /**
   * @brief 恢复运行
   */
  void Resume();

  /**
   * @brief 获取事件中心
   */
  constexpr const std::shared_ptr<EventCenter> &GetEventCenter() const { return event_center_; }

  /**
   * @brief 设置动态化框架输出
   */
  void SetOutput(std::shared_ptr<Output> output);

  /**
   * @brief 获取Output对象
   */
  std::shared_ptr<Output> GetOutput() const { return output_; }

  /**
   * @brief 标记需要构建RenderObject树
   * Pipeline将在下一个vsync信号来临时构建RenderObject树
   */
  void SetNeedsBuild();

  /**
   * @brief 标记需要Paint
   * Pipeline将在下一个vsync信号来临时从根节点Paint
   */
  void SetNeedsPaint();

  /**
   * @brief 设置`构建RenderObject树`的实现
   * @param func `构建RenderObject树`的实现
   */
  void SetBuildFunction(const std::function<std::shared_ptr<RenderTree>()> &func);

  /**
   * @brief 获取UITaskRunner
   */
  std::shared_ptr<TaskRunner> GetUITaskRunner() const { return scheduler_->GetUITaskRunner(); }

  /**
   * @brief 获取RasterTaskRunner
   */
  std::shared_ptr<TaskRunner> GetRasterTaskRunner() const { return scheduler_->GetRasterTaskRunner(); }

  /**
   * @brief 获取IOTaskRunner
   */
  std::shared_ptr<TaskRunner> GetIOTaskRunner() const { return scheduler_->GetIOTaskRunner(); }

  /**
   * @brief 获取PlatformTaskRunner
   */
  std::shared_ptr<TaskRunner> GetPlatformTaskRunner() const { return scheduler_->GetPlatformTaskRunner(); }

  /**
   * @brief 获取scheduler
   */
  std::shared_ptr<Scheduler> GetScheduler() const { return scheduler_; }

  /**
   * @brief 获取RenderPipeline
   * @return RenderPipeline
   */
  std::shared_ptr<RenderPipeline> GetRenderPipeline() const;

  /*
   * @brief 内核是否可以响应手势事件，如果相应在 Native RenderObject
   * 是属于内核不能响应事件状态
   * @param event 事件数据
   * @return
   */
  bool CanConsumeEvent(const std::shared_ptr<PointerEvent> &pointer_event);

  /**
   * @brief 获取平台View参数
   * @return 平台View参数
   */
  constexpr const ViewportMetrics &GetViewportMetrics() { return viewport_metrics_; }

  /**
   * @brief 获取本引擎实例所使用的UI系统虚拟时钟
   */
  std::shared_ptr<EngineClock> GetClock() const { return scheduler_->GetClock(); }

  /**
   * @brief 获取最新的截屏Bitmap
   * @param callback    截屏回调，回传SkImage，在GPUTaskRunner中被执行
   * @param down_scale  缩小系数
   */
  void GetLastScreenshot(std::function<void(std::shared_ptr<Texture> screenshot)> callback, float pixel_ratio = 1.0f);

  /**
   * @brief 返回性能数据检测器Profiler实例
   */
  std::shared_ptr<Profiler> GetProfiler() const { return profiler_; }

  /**
   * @brief 将RenderTree生成一张Picture
   */
  std::shared_ptr<Picture> FlattenRenderObject(const std::shared_ptr<RenderObject> &object);

  /**
   * @brief 光栅化Picture，生成图片
   * @param picture Picture
   * @param callback 完成回调
   * @param rect 导出图片的区域
   * @param pixel_ratio 缩放系数
   */
  void RasterizePicture(const std::shared_ptr<Picture> &picture,
                        const std::function<void(std::shared_ptr<Texture>)> &callback, TRect rect,
                        double pixel_ratio = 1.0);

  /**
   * 获取输入管理模块
   * @return
   */
  std::shared_ptr<TextInput> GetTextInput();

  /**
   * @brief 更新资源设备
   */
  void UpdateResourceDevice();

  /**
   * @brief 切换render backend
   */
  void SwitchRenderBackend(RenderBackend render_backend);

 protected:
  void Init() override;

 private:
  Shell(const std::shared_ptr<Platform> &platform, const ShellConfig &config);

  void InitThreadLocalShellInstances();
  void AddEventListeners();
  void RemoveEventListeners();
  void SetEmbeddedSnapshot(const std::shared_ptr<Layer> &layer);
  std::shared_ptr<RenderObject> PrepareFlatten(const std::shared_ptr<RenderObject> &object);

#if TDF_SERVICE_ENABLED
  void InitDevTools();
#endif

  bool is_started_ = false;
  bool is_terminated_ = false;
  ShellConfig config_;
  std::shared_ptr<EventCenter> event_center_;
  std::unordered_map<uint64_t, Type> event_listeners_;
  std::shared_ptr<Platform> platform_;
  std::shared_ptr<Device> resource_device_;
  std::shared_ptr<Output> output_;
  std::shared_ptr<Scheduler> scheduler_;
  std::shared_ptr<RenderPipeline> render_pipeline_;
  ViewportMetrics viewport_metrics_;
  std::shared_ptr<PointerHandler> pointer_handler_;
  std::mutex consume_event_mutex_;
  std::condition_variable consume_event_cv_;
  std::shared_ptr<TextInput> text_input_;
  std::shared_ptr<ShortcutManager> shortcut_manager_;
  std::shared_ptr<Profiler> profiler_;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
