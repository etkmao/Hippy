//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#import <UIKit/UIKit.h>
#include "core/common/macro.h"
#include "core/platform/common/vsync_task_driver.h"

@class TDFVSyncClient;

namespace tdfcore {
class VSyncTaskDriverIOS final : public VSyncTaskDriver {
 public:
  explicit VSyncTaskDriverIOS(const Callback& callback = nullptr);
  ~VSyncTaskDriverIOS() override;

  API_AVAILABLE(ios(15.0))
  void SetFrameRateRange(CAFrameRateRange range);

 protected:
  void OnStart() override;
  void OnStop() override;

 private:
  TDFVSyncClient* client_;

  TDF_DISALLOW_COPY_AND_ASSIGN(VSyncTaskDriverIOS);
};

/**
 * @brief Vsync信号模拟器
 * vsync模拟器将不受系统vsync的限制，可定制vsync信号的时间间隔
 */
class FixedIntervalTaskDriverIOS final : public VSyncTaskDriver {
 public:
  FixedIntervalTaskDriverIOS(int64_t interval, const Callback& callback = nullptr);

 protected:
  void OnStart() override;
  void OnStop() override;

 private:
  int64_t interval_;
  dispatch_source_t timer_;

  TDF_DISALLOW_COPY_AND_ASSIGN(FixedIntervalTaskDriverIOS);
};

}  // namespace tdfcore
