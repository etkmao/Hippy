//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/common/macro.h"
#include "core/platform/common/vsync_task_driver.h"

@class TDFVSyncClientMacOS;

namespace tdfcore {
class VSyncTaskDriverMacOS final : public VSyncTaskDriver {
 public:
  /**
   * @brief VSyncTaskDriverMacOS使用CVDisplayLink, CVDisplayLink会使用单独的线程
   * @param callback 回调
   * @param fire_on_main_thread 是否在主线程回调, 默认false
   */
  explicit VSyncTaskDriverMacOS(const Callback& callback = nullptr, bool fire_on_main_thread = false);

 protected:
  void OnStart() override;
  void OnStop() override;

 private:
  bool fire_on_main_thread_;
  TDFVSyncClientMacOS* client_;

  TDF_DISALLOW_COPY_AND_ASSIGN(VSyncTaskDriverMacOS);
};
}  // namespace tdfcore
