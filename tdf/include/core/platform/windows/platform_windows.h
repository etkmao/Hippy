//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/common/time.h"
#include "core/engine/schedule/task_runner.h"
#include "core/platform/common/platform.h"
#include "core/platform/windows/embedded_view/embedded_view_register.h"
#include "core/platform/windows/windows_define.h"

namespace tdfcore {
/**
 * @brief 提供Win平台相关能力
 */
class PlatformWindows : public Platform {
 public:
  PlatformWindows(HWND window_handle);

  std::shared_ptr<TaskDriver> CreateVSyncTaskDriver() override;

  std::shared_ptr<TaskDriver> CreateRunLoopTaskDriver() override;

  std::shared_ptr<PlatformEmbeddedView> CreateEmbeddedView(const std::string &view_type, int64_t view_id,
                                                           const PlatformEmbeddedView::PropsMap &propsMap) override;

  std::shared_ptr<PlatformView> CreateOverlayView(int64_t view_id,
                                                  const std::shared_ptr<Scheduler> &scheduler) override;

  std::shared_ptr<ChannelRuntime> GetChannelRuntime() override;

  std::shared_ptr<Menu> GetMenu() override;

  std::shared_ptr<PlatformWindow> MakeWindow(const std::string_view &title, const TRect &frame) override;

  void SetPlatformTaskRunner(const std::shared_ptr<TaskRunner> &platform_task_runner) {
    platform_task_runner_ = platform_task_runner;
  }
  void SetEmbeddedViewRegister(const std::shared_ptr<EmbeddedViewRegister> &embedded_view_register) {
    embedded_view_register_ = embedded_view_register;
  }

  bool IsMainThread() const override;
  std::string GetAppRootPath() const override { return ""; }
  std::string GetCacheDirectoryPath() const override { return ""; }
  std::shared_ptr<FileManager> GetFileManager() override { return file_manager_; }
  float GetDeviceRatio() const override { return 0; }
  void SetMouseCursorStyle(SystemMouseCursor system_mouse_cursor) override;

 private:
  std::shared_ptr<Keyboard> CreateKeyboard() override;
  std::shared_ptr<Clipboard> CreateClipboard() override;
  std::shared_ptr<FilePicker> CreateFilePicker() override;
  std::shared_ptr<ChannelRuntime> channel_runtime_;
  std::shared_ptr<FileManager> file_manager_;
  std::shared_ptr<TaskRunner> platform_task_runner_;
  std::shared_ptr<Menu> menu_;
  std::shared_ptr<EmbeddedViewRegister> embedded_view_register_;
  HWND window_handle_;
  std::thread::id main_thread_id_;
};

}  // namespace tdfcore
