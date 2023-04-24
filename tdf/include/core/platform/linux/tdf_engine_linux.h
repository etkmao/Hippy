//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include "core/engine/schedule/signal_task_driver.h"
#include "core/engine/schedule/task_runner.h"
#include "core/platform/linux/input_dispatcher.h"
#include "core/tdfi/shell.h"

namespace tdfcore {

class TDFEngineLinux : public Object, public std::enable_shared_from_this<TDFEngineLinux> {
 public:
  ~TDFEngineLinux() override;
  TDFEngineLinux();

  /**
   * @brief 启动内核Pipeline
   */
  void Start() const;
  /**
   * @brief 暂停内核PipeLine
   */
  void Pause() const;
  /**
   * @brief 重启内核Pipeline
   */
  void Resume() const;
  /**
   * @brief 停止内核Pipeline
   */
  void Stop() const;

  /**
   * 获取引擎Pipeline任务的TaskDriver
   * @return
   */
  std::shared_ptr<SignalTaskDriver> GetPipelineDriver() const;

  /**
   * @brief 注册对于鼠标监听，TODO(vimerzhao): 键盘
   */
  void SetupInputListener(PointerDataUpdateListener listener = nullptr);

  std::shared_ptr<Shell> GetShell() const { return shell_; }

  void SetShell(const std::shared_ptr<Shell>& shell) { shell_ = shell; }

 private:
  std::shared_ptr<Shell> shell_;
  std::shared_ptr<InputDispatcher> input_dispatcher_;
  std::shared_ptr<TaskRunner> task_runner_;
  std::shared_ptr<TaskDriver> input_dispatcher_driver_;
};

}  // namespace tdfcore
