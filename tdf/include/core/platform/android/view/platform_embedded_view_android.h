//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <string>

#include "core/platform/android/jni/jni_df_helper.h"
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/common/view/platform_embedded_view.h"
#include "core/platform/common/viewport_metrics.h"

namespace tdfcore {
/**
 * @brief 对应嵌入显示层的Android NativeView,例如显示层嵌入的Android WebView
 */
class PlatformEmbeddedViewAndroid : public PlatformEmbeddedView {
 public:
  PlatformEmbeddedViewAndroid(JavaWeakGlobalRef<jobject> dt_jni_ref,
                              ViewportMetrics &viewport_metrics,
                              const std::string &view_type,
                              int64_t view_id,
                              const PropsMap &props_map,
                              const std::shared_ptr<ChannelRuntime>& channel_runtime);
  ~PlatformEmbeddedViewAndroid();
  void UpdateProps(const PropsMap &props) override;
  void UpdateMutatorsToNative(const MutatorsStack &mutators_stack) override;
  void SetFrame(const TRect &frame) override;
  TRect GetFrame() override;
  void InsertView(const std::shared_ptr<PlatformView> &platform_view, int64_t index) override;
  void RemoveView(const std::shared_ptr<PlatformView> &platform_view) override;
  void *GetViewPointer() override;
  std::future<std::shared_ptr<Bitmap>> MakeSnapshot() override;
 private:
  JavaWeakGlobalRef<jobject> global_jni_weak_ref_;
  JavaGlobalRef<jobject> embed_view_ref_;
  JavaGlobalRef<jobject> embed_view_controller_;
  ViewportMetrics viewport_metrics_;
};

}  // namespace tdfcore