//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <android/looper.h>
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/common/vsync_task_driver.h"

namespace tdfcore {
/**
 * @brief Android平台vsyncDriver基类，抽象vsync驱动信号回调方法
 */
class VSyncTaskDriverAndroidBase : public VSyncTaskDriver {
 public:
  /**
   * @brief vsyncDriver驱动信号回调方法
   */
  virtual void OnDrive() = 0;

 protected:
  tdfcore::JavaWeakGlobalRef<jobject> df_jni_weak_ref_;
  ALooper* looper_;

  explicit VSyncTaskDriverAndroidBase(JavaWeakGlobalRef<jobject> weak_ref,
                                      ALooper* looper = ALooper_forThread(),
                                      const Callback& callback = nullptr)
      : VSyncTaskDriver(callback), df_jni_weak_ref_(weak_ref), looper_(looper) {}

  TDF_DISALLOW_COPY_AND_ASSIGN(VSyncTaskDriverAndroidBase);
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore