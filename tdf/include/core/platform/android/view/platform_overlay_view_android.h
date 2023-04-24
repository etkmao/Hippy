//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/android/view/platform_view_android.h"

namespace tdfcore {

/**
 * @brief 对应Android平台的Overlay SurfaceView实现
 */
class PlatformOverlayViewAndroid : public PlatformViewAndroid {
 public:
  explicit PlatformOverlayViewAndroid(JavaWeakGlobalRef<jobject> df_jni,
                                      int64_t embedded_view_id, const std::shared_ptr<Scheduler> &scheduler);
  ~PlatformOverlayViewAndroid() override;

  void SetFrame(const TRect &frame) override;
  TRect GetFrame() override;
  void InsertView(const std::shared_ptr<PlatformView> &platform_view, int64_t index) override;
  void RemoveView(const std::shared_ptr<PlatformView> &platform_view) override;
  void *GetViewPointer() override;

  void SetViewId(int64_t view_id) {
    view_id_ = view_id;
  }
  constexpr int64_t GetViewId() const {
    return view_id_;
  }
 private:
  const JavaWeakGlobalRef<jobject> df_jni_object_;
  JavaGlobalRef<jobject> view_reference_obj_;
  int64_t view_id_;
};
}  // namespace tdfcore