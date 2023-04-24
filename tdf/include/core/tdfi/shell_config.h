//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/schedule/scheduler.h"
#include "core/platform/common/viewport_metrics.h"
#include "core/platform/common/vsync_task_driver.h"

namespace tdfcore {

/**
 * @brief 动态化框架配置
 */
struct ShellConfig {
  /**
   * @brief 线程调度相关的配置
   */
  std::shared_ptr<Scheduler> scheduler;

  /**
   * @brief 视图参数
   */
  ViewportMetrics viewport_metrics;

  // ResourceDevice 如果基于EGLWindowSurface创建，则需要一个窗口句柄
  void* native_window = nullptr;
};

}  // namespace tdfcore
