//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <jni.h>
#include "core/platform/android/vsync/vsync_task_driver_android_base.h"

namespace tdfcore {

/**
 * @brief Android平台Vsync机制实现类
 */
class VSyncTaskDriverAndroid final : public VSyncTaskDriverAndroidBase {
 public:
  ~VSyncTaskDriverAndroid() override = default;

  /**
   * @brief 构造函数
   * @param looper 发起VSync回调的线程Looper（通常为平台主线程）；默认值为创建实例时所在线程的Looper
   */
  explicit VSyncTaskDriverAndroid(tdfcore::JavaWeakGlobalRef<jobject> weak_ref,
                                  ALooper* looper = ALooper_forThread(), const Callback& callback = nullptr);

 protected:
  void OnStart() override;
  void OnStop() override;
  void OnDrive() override;

  /**
   * @brief 从NDK监听VSync，监听成功返回true
   * @param is_from_on_start 是否从OnStart触发
   */
  bool TryListenVSyncFromNDK(bool is_from_on_start = false);

  bool is_linstening_vsync_from_ndk_ = false;

  TDF_DISALLOW_COPY_AND_ASSIGN(VSyncTaskDriverAndroid);
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
