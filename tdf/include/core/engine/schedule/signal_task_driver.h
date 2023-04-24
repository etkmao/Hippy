//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <list>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include "core/engine/schedule/task_driver.h"

namespace tdfcore {

/**
 * @brief 由外部信号源驱动taskRunner,信号源到来调用SignalDrive()方法后会驱动TaskRunner运行一次
 */
class SignalTaskDriver : public TaskDriver {
 public:
  ~SignalTaskDriver() override = default;

  /**
   * @brief 构造函数
   */
  explicit SignalTaskDriver(const Callback &callback = nullptr,
                            const std::function<void()> &wake_up_callback = nullptr);

  /**
   * @brief 唤醒TaskDriver的阻塞态，使其尽快执行一次驱动
   */
  void WakeUp() override;

  /**
   * @brief 外部驱动信号源驱动接口，每次调用一次会驱动一次。方法在哪个线程调用，任务将在这个线程中执行
   */
  void SignalDrive();

 protected:
  void OnStop() override;

 private:
  std::function<void()> wake_up_callback_;
};

}  // namespace tdfcore
