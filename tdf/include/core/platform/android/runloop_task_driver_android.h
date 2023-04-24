//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <android/looper.h>
#include <unistd.h>
#include <string>
#include "core/common/time.h"
#include "core/engine/schedule/task_driver.h"

namespace tdfcore {

class RunLoopTaskDriverAndroid : public TaskDriver {
 public:
  ~RunLoopTaskDriverAndroid() override = default;

  /**
   * @brief 构造函数
   * @param looper RunLoop所在线程的Looper（通常为平台主线程）；默认值为创建实例时所在线程的Looper
   */
  explicit RunLoopTaskDriverAndroid(ALooper *looper = ALooper_forThread(),
                                    const Callback& callback = nullptr);

  void WakeUp() override;

 protected:
  void OnStop() override;

 private:
  void OnEventFired();
  bool TimerRearm(int fd, const TDuration &duration);
  bool TimerDrain(int fd);

 private:
  ALooper *looper_;
  int timer_fd_;
};

}  // namespace tdfcore
