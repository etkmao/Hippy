//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/platform/android/event/platform_event_android.h"
#include "core/platform/android/jni/jni_df_helper.h"
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/tdfi/shell.h"

namespace tdfcore {

class PlatformEventHandler : public Object, public std::enable_shared_from_this<PlatformEventHandler> {
 public:
  ~PlatformEventHandler() override;
  void Init() override;

  /**
   * @brief 注册JNI回调方法
   */
  static bool BindJNI(JNIEnv *env);

 protected:
  PlatformEventHandler(const JavaWeakGlobalRef<jobject> &native_object, const std::weak_ptr<Shell> &shell);

  /**
   * @brief Android侧的TDFPlatformEventHandler通过JNI发送到C++侧的tdfcore::PlatformEventHandler的事件数据的类型枚举
   * @note 请保持此处的枚举值与TDFPlatformEventHandler.java中定义的常量的一致性
   */
  enum class EventDataType : jint {
    kUnknown = 0,
    kMotion = 1,
    kKey = 2,
    kViewport = 3,
  };

 private:
  JavaWeakGlobalRef<jobject> native_object_;
  std::weak_ptr<Shell> shell_;

  static void HandleEventData(JNIEnv *env, jobject caller, jlong object_id, jint event_type, jobject data, jint size);
  void HandleEvent(const AndroidMotionEvent &event);
  void HandleEvent(const AndroidKeyEvent &event);
  void HandleEvent(const ViewportMetrics &metrics);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
