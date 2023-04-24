//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <android/looper.h>

#include "core/platform/android/file_manager_android.h"
#include "core/platform/android/jni/jni_helper.h"
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/android/runloop_task_driver_android.h"
#include "core/platform/common/file_picker.h"
#include "core/platform/common/platform.h"

namespace tdfcore {

/**
 * @brief Platform Android模块相关的具体实现
 */
class PlatformAndroid : public Platform {
 public:
  ~PlatformAndroid() override;

  std::shared_ptr<TaskDriver> CreateVSyncTaskDriver() override;

  std::shared_ptr<TaskDriver> CreateRunLoopTaskDriver() override;

  bool IsMainThread() const override;

  std::string GetAppRootPath() const override;

  std::string GetCacheDirectoryPath() const override;

  std::shared_ptr<FileManager> GetFileManager() override;

  std::shared_ptr<PlatformEmbeddedView> CreateEmbeddedView(const std::string &view_type, int64_t view_id,
                                                           const PlatformEmbeddedView::PropsMap &propsMap) override;

  std::shared_ptr<PlatformView> CreateOverlayView(int64_t view_id,
                                                  const std::shared_ptr<Scheduler> &scheduler) override;

  void SetViewPortMetrics(const ViewportMetrics &viewport_metrics) { viewport_metrics_ = viewport_metrics; }

  std::shared_ptr<ChannelRuntime> GetChannelRuntime() override;

  void SetMouseCursorStyle(SystemMouseCursor system_mouse_cursor) override {}
  const JavaWeakGlobalRef<jobject> GetGlobalJniObject() const { return global_jni_object_; }

 protected:
  explicit PlatformAndroid(JavaWeakGlobalRef<jobject> global_jni_weak_ref);
  float GetDeviceRatio() const override;

 private:
  std::shared_ptr<Keyboard> CreateKeyboard() override;

  std::shared_ptr<Clipboard> CreateClipboard() override;

  std::shared_ptr<FilePicker> CreateFilePicker() override;

 private:
  const JavaWeakGlobalRef<jobject> global_jni_object_;
  ALooper *main_looper_ = nullptr;
  std::shared_ptr<FileManagerAndroid> file_manager_android_ = nullptr;
  jobject file_manager_file_info_jobject;
  ViewportMetrics viewport_metrics_;
  std::shared_ptr<ChannelRuntime> channel_runtime_android_ = nullptr;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore