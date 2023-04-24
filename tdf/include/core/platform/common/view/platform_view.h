//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/object.h"
#include "core/common/rect.h"
#include "core/engine/graphics/device.h"
#include "core/engine/graphics/surface.h"
#include "core/engine/render/render_context.h"

namespace tgfx {
class Window;
}

namespace tdfcore {
class Scheduler;
/**
 * @brief PlatformView抽象了平台视图的能力，抽象类（由各个平台实现）
 */
class PlatformView : public Object, public std::enable_shared_from_this<PlatformView> {
 public:
  explicit PlatformView(const std::shared_ptr<Scheduler> &scheduler);
  ~PlatformView() override;

  /**
   * @brief 设置大小和位置
   * @param frame view的大小和位置
   */
  virtual void SetFrame(const TRect &frame) = 0;

  /**
   * @brief 获取view的大小和位置
   */
  virtual TRect GetFrame() = 0;

  /**
   * @brief 外部原因发生frame变化需要进行通知
   */
  virtual void NotifyFrameChange();

  /**
   * @brief 将View实例插入指定位置
   * @param platform_view 被插入的view
   */
  virtual void InsertView(const std::shared_ptr<PlatformView> &platform_view, int64_t index) = 0;

  /**
   * @brief 删除View
   * @param platform_view 被删除的view
   */
  virtual void RemoveView(const std::shared_ptr<PlatformView> &platform_view) = 0;

  /**
   * @brief 获取内部平台实际View的指针
   */
  virtual void *GetViewPointer() = 0;

  /**
   * @brief 初始化tgfx::Window
   */
  virtual void InitWindow(const std::shared_ptr<Device> &shared_device) {}

  /***
   * @brief 上屏
   */
  virtual void Present(GraphicsContext *graphics_context);

  /**
   * @brief 返回scheduler
   * @return
   */
  std::shared_ptr<Scheduler> GetScheduler() const { return scheduler_; }

  /**
   * @brief 返回surface
   * @return
   */
  std::shared_ptr<Surface> GetSurface() const { return surface_; }

  /**
   * @brief 返回device
   * @return
   */
  std::shared_ptr<Device> GetDevice() const { return device_; }

  void UpdateDevice();

 protected:
  /**
   * @brief 当前实例绑定的平台窗口(View)销毁时调用
   */
  void OnNativeWindowDestroy();

  std::shared_ptr<Scheduler> scheduler_;
  std::shared_ptr<tgfx::Window> window_;
  std::shared_ptr<Device> device_;
  std::shared_ptr<Surface> surface_;
  std::atomic_flag need_present_ = ATOMIC_FLAG_INIT;
};
}  // namespace tdfcore
