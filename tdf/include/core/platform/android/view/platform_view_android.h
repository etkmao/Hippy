//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <string>
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/android/output/android_window.h"
#include "core/platform/common/view/platform_view.h"

namespace tdfcore {

/**
 * @brief 对应Adroid Native 侧 TDFView
 */
class PlatformViewAndroid : public PlatformView {
 public:
  PlatformViewAndroid(JavaWeakGlobalRef<jobject> java_weak_global_object, const std::shared_ptr<Scheduler> &scheduler);

  virtual ~PlatformViewAndroid();

  /**
   * @brief Android native端Surface创建回调，因为是异步回调所以在这里暂存一下
   * @param android_window
   */
  void OnSurfaceCreated(const std::shared_ptr<AndroidWindow> &android_window);
  /**
   * @brief Android侧Surface Size发生改变的回调
   * @param width
   * @param height
   */
  void OnSurfaceSizeChanged(int width, int height);

  void InitWindow(const std::shared_ptr<Device> &shared_device) override;

  /**
   * @brief 销毁Surface回调
   */
  void OnSurfaceDestroyed();

  /**
   * @brief 返回Android Window
   * @return
   */
  const std::shared_ptr<AndroidWindow> GetNativeWindow() const { return android_window_; }

  /**
   * @brief 以下方法是Platform通过JNI回调到平台侧的实现
   */
  void SetFrame(const TRect &frame) override;
  TRect GetFrame() override;
  void InsertView(const std::shared_ptr<PlatformView> &platform_view, int64_t index) override;
  void RemoveView(const std::shared_ptr<PlatformView> &platform_view) override;
  void *GetViewPointer() override;
  void Present(GraphicsContext *graphics_context) override;

 protected:
  const JavaWeakGlobalRef<jobject> df_jni_object_;
  JavaGlobalRef<jobject> view_reference_obj_;
  std::shared_ptr<AndroidWindow> android_window_;
};

}  // namespace tdfcore