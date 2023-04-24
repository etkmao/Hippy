//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "core/engine/schedule/task_driver.h"

namespace tdfcore {

class RunLoopTaskDriverDarwin : public TaskDriver {
 public:
  ~RunLoopTaskDriverDarwin() override;
  void WakeUp() override;

 protected:
  explicit RunLoopTaskDriverDarwin(NSRunLoop *run_loop = NSRunLoop.mainRunLoop);
  void OnStop() override;

 private:
  NSRunLoop *run_loop_ = nil;
  CFRunLoopTimerRef timer_ = NULL;

  static void OnTimerFire(CFRunLoopTimerRef timer, RunLoopTaskDriverDarwin *task_driver);
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
