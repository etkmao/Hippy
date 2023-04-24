//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include "core/common/time.h"
#include "core/tdfi/shell.h"

namespace tdfcore {

using TimerCallback = std::function<void(int64_t tick)>;

class Timer : public Object, public std::enable_shared_from_this<Timer> {
 public:
  ~Timer() override = default;

  /**
   * @brief 获取是计时器否激活
   * @return 是否激活
   */
  bool IsActive() const;

  /**
   * @brief 启动计时器
   */
  void Start();

  /**
   * @brief 取消计时器
   */
  void Cancel();

 private:
  Timer(TDuration interval, bool repeats, const TimerCallback &callback,
        std::weak_ptr<Shell> shell = Shell::GetCurrent());

  void Schedule();
  bool is_active_ = false;
  bool repeats_;
  TDuration interval_;
  TimePoint start_time_;
  int64_t callback_counter_ = 0;
  TimerCallback on_timer_;
  std::weak_ptr<Shell> shell_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
