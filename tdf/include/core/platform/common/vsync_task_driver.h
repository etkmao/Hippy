//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include "core/common/macro.h"
#include "core/common/memory.h"
#include "core/common/object.h"
#include "core/common/time.h"
#include "core/engine/schedule/task_driver.h"

namespace tdfcore {

/**
 * @brief vsync信号驱动器抽象类，为内核屏蔽各个平台的VsyncTaskDriver机制细节
 */
class VSyncTaskDriver : public TaskDriver {
 public:
  ~VSyncTaskDriver() override;
  void WakeUp() override;

 protected:
  explicit VSyncTaskDriver(const Callback &callback = nullptr);

  /**
   * @brief 驱动信号已到达，执行相应逻辑
   */
  void FireCallback();

  TDF_DISALLOW_COPY_AND_ASSIGN(VSyncTaskDriver);
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
