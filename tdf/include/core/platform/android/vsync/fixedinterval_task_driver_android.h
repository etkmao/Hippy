//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <jni.h>
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/android/vsync/vsync_task_driver_android_base.h"

namespace tdfcore {

/**
 * @brief Android平台固定驱动频率驱动器实现类
 */
class FixedIntervalTaskDriverAndroid final : public VSyncTaskDriverAndroidBase {
 public:
  ~FixedIntervalTaskDriverAndroid() override = default;

 protected:
  /**
   * @brief 构造函数
   * @param looper 发起VSync回调的线程Looper（通常为平台主线程）；默认值为创建实例时所在线程的Looper
   */
  FixedIntervalTaskDriverAndroid(const TDuration &vsync_driver_interval, tdfcore::JavaWeakGlobalRef<jobject> weak_ref,
                                 ALooper* looper = ALooper_forThread(), const Callback &callback = nullptr);
  void OnStart() override;
  void OnStop() override;
  void OnDrive() override;

 private:
  TDuration vsync_driver_interval_;

  TDF_DISALLOW_COPY_AND_ASSIGN(FixedIntervalTaskDriverAndroid);
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore