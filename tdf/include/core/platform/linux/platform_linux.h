//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include "core/engine/graphics/device.h"
#include "core/engine/schedule/signal_task_driver.h"
#include "core/platform/common/keyboard/keyboard.h"
#include "core/platform/common/platform.h"

namespace tdfcore {

enum class WindowType { kX11, kWayland, kNone };

class PlatformLinux : public Platform {
 public:
  ~PlatformLinux() override;

  std::shared_ptr<TaskDriver> CreateVSyncTaskDriver() override;

  std::shared_ptr<FileManager> GetFileManager() override;

  bool IsMainThread() const override { return true; }

  std::string GetAppRootPath() const override { return ""; }

  std::string GetCacheDirectoryPath() const override { return ""; }

  float GetDeviceRatio() const override { return 1.0; }

  std::shared_ptr<PlatformEmbeddedView> CreateEmbeddedView(const std::string& view_type, int64_t view_id,
                                                           const PlatformEmbeddedView::PropsMap& propsMap) override {
    return nullptr;
  }

  std::shared_ptr<PlatformView> CreateOverlayView(int64_t view_id,
                                                  const std::shared_ptr<Scheduler>& scheduler) override {
    return nullptr;
  }

  std::shared_ptr<ChannelRuntime> GetChannelRuntime() override;

  std::shared_ptr<Keyboard> CreateKeyboard() override;

  std::shared_ptr<Clipboard> CreateClipboard() override { return nullptr; }

  std::shared_ptr<FilePicker> CreateFilePicker() override { return nullptr; }

  void SetMouseCursorStyle(SystemMouseCursor system_mouse_cursor) override {}

  WindowType GetWindowType() { return type_; }

  // 暂时使用 epoll+timerfd 模拟 Vsync
  void StartVsyncListening();

 private:
  PlatformLinux();

  std::shared_ptr<SignalTaskDriver> task_driver_;
  int epoll_fd_ = -1;

  WindowType type_;
  std::shared_ptr<ChannelRuntime> channel_runtime_;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
