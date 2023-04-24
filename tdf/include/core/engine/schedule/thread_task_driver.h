//
// Copyright © 2020 com.tencent. All rights reserved.
//

#pragma once

#include <string>
#include <thread>
#include "core/engine/schedule/task_driver.h"

namespace tdfcore {

/**
 * @brief ThreadTaskDriver 会单独建立一个线程作为驱动
 */
class ThreadTaskDriver : public TaskDriver {
 public:
  ~ThreadTaskDriver() override = default;
  explicit ThreadTaskDriver(const Callback &callback = nullptr);
  void WakeUp() override;

 protected:
  void OnStart() override;
  void OnStop() override;

 private:
  std::thread thread_;
  std::shared_ptr<WaitableObject> thread_waiter_;
};

}  // namespace tdfcore
