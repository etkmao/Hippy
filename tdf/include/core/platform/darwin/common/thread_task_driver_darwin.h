//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#import <Foundation/Foundation.h>
#include "core/engine/schedule/task_driver.h"

namespace tdfcore {

class ThreadTaskDriverDarwin : public TaskDriver {
 public:
  ~ThreadTaskDriverDarwin() override;
  void SetName(const std::string &driver_name) override;
  void WakeUp() override;

 protected:
  explicit ThreadTaskDriverDarwin(const Callback &callback = nullptr);
  void OnStart() override;
  void OnStop() override;

 private:
  NSThread *thread_ = nil;
  NSRunLoop *run_loop_ = nil;
  CFRunLoopTimerRef timer_ = NULL;

  void OnRun();
  static void OnTimerFire(CFRunLoopTimerRef timer, ThreadTaskDriverDarwin *task_driver);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
