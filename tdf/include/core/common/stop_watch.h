//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <functional>
#include <iostream>
#include <string>
#include "core/common/time.h"

namespace tdfcore {

/**
 * @brief 计时器；用于统计一个代码作用域的执行时间
 * @detail 当实例创建时，立刻开始计时，当实例析构时，结束计时，同时发起callback回调
 */
class ScopedStopWatch : public Object {
 public:
  ~ScopedStopWatch() override;

  using Callback = std::function<void(const RealTimePoint &begin_time, const RealTimePoint &end_time)>;

  explicit ScopedStopWatch(const Callback &callback);

 private:
  Callback callback_;
  RealTimePoint begin_time_;
  RealTimePoint end_time_;
};

}  // namespace tdfcore
