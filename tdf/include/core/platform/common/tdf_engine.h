//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include "core/common/object.h"
#include "core/engine/schedule/task_driver.h"
#include "core/platform/common/output/texture_output.h"
#include "core/platform/common/output/texture_to_view_output.h"
#include "core/platform/common/output/view_output.h"
#include "core/platform/common/platform.h"
#include "core/platform/common/view/platform_view.h"
#include "core/tdfi/shell.h"
#include "core/tdfi/shell_config.h"

namespace tdfcore {

struct TDFEngineConfig {
  enum class OutputType {
    View,          // 输出到平台窗口，对应一个原生View
    Texture,       // 输出到一个纹理
    TextureToView  // 先输出到纹理，再把纹理渲染到原生View
  };
  enum class ThreadModel {
    Default,      // UI一个线程，Raster/Platform共用线程
    Performance,  // UI/Raster/Platform各一个线程
    Sync          // UI/Raster/Platform共用一个线程
  };

  OutputType output_type_;
  ThreadModel thread_mode_;
  bool is_simulate_vsync = false;
  TDuration pipeline_driver_interval = std::chrono::milliseconds(8);
};

/**
 * @brief
 */
class TDFEngine : public Object {
 public:
  virtual ~TDFEngine();

  /**
   * @brief 启动内核Pipeline
   */
  void Start();

  /**
   * @brief 重新恢复内核Pipeline
   */
  void Resume();

  /**
   * @brief 暂停内核Pipeline
   */
  void Pause();

  /**
   * @brief 通知内核应用处于不可见状态
   */
  void OnAppPause();

  /**
   * @brief 通知内核应用处于可见状态
   */
  void OnAppResume();

  /**
   * @brief 通知内核应用处于不可交互状态
   */
  void OnAppInactive();

  /**
   * @brief 通知内核应用即将销毁
   */
  void OnAppDestroy();

  /**
   * @brief 分发Android平台手势数据
   * @param packet Android平台手势数据buffer holder
   */
  // void DispatchEventPacket(std::unique_ptr<PointerBufferHolder> packet);

  /**
   * @brief 更新ViewportMetrics
   * @param viewport_metrics 新的ViewportMetrics
   */
  virtual void UpdateViewportMetrics(const ViewportMetrics &viewport_metrics);

  /**
   * @brief 系统内存紧张时触发调用，释放内存减轻应用内存压力
   */
  void NotifyLowMemoryWarning() const;

  /**
   * @brief 获取shell实例
   * @return shell实例
   */
  std::shared_ptr<Shell> GetShell() const { return shell_; }

  template <class T>
  std::shared_ptr<T> GetPlatformView() const {
    static_assert(std::is_base_of<PlatformView, T>::value, "Invalid Target PlatformView Type");
    return std::static_pointer_cast<T>(platform_view_);
  }

  template <class T>
  const std::shared_ptr<T> GetPlatform() const {
    static_assert(std::is_base_of<Platform, T>::value, "Invalid Target PlatformView Type");
    return std::static_pointer_cast<T>(platform_);
  }
  std::shared_ptr<EmbeddedViewManager> GetEmbeddedViewManager() const { return embedded_view_manager_; }

  const std::shared_ptr<Output> GetOutput() const { return output_; }

 protected:
  explicit TDFEngine(const TDFEngineConfig &engine_config);

  virtual void Init() override;

  virtual std::shared_ptr<Platform> MakePlatform() = 0;

  virtual std::shared_ptr<PlatformView> MakePlatformView() = 0;

  virtual std::shared_ptr<TaskDriver> MakePipelineDriver() = 0;

  virtual std::shared_ptr<Scheduler> MakeScheduler();

  std::shared_ptr<Output> MakeOutput();

  const TDFEngineConfig engine_config_;

 private:
  std::shared_ptr<Shell> shell_;
  std::shared_ptr<Platform> platform_;
  std::shared_ptr<PlatformView> platform_view_;
  std::shared_ptr<Output> output_;
  std::shared_ptr<EmbeddedViewManager> embedded_view_manager_;
};

}  // namespace tdfcore
