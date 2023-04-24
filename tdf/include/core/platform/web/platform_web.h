//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/graphics/device.h"
#include "core/engine/schedule/thread_task_driver.h"
#include "core/platform/common/keyboard/keyboard.h"
#include "core/platform/common/platform.h"
#include "core/platform/web/keyboard/keyboard_web.h"
#include "core/platform/web/platform_view_web.h"

namespace tdfcore {

class PlatformWeb : public Platform {
 public:
  ~PlatformWeb() override = default;

  std::shared_ptr<TaskDriver> CreateVSyncTaskDriver() override { return TDF_MAKE_SHARED(ThreadTaskDriver); };

  std::shared_ptr<FileManager> GetFileManager() override { return nullptr; }

  bool IsMainThread() const override { return true; }

  std::string GetAppRootPath() const override { return ""; }

  std::string GetCacheDirectoryPath() const override { return ""; }

  float GetDeviceRatio() const override;

  std::shared_ptr<PlatformEmbeddedView> CreateEmbeddedView(const std::string& view_type, int64_t view_id,
                                                           const PlatformEmbeddedView::PropsMap& propsMap) override;

  std::shared_ptr<PlatformView> CreateOverlayView(int64_t view_id,
                                                  const std::shared_ptr<Scheduler>& scheduler) override;

  std::shared_ptr<ChannelRuntime> GetChannelRuntime() override;

  std::shared_ptr<Keyboard> CreateKeyboard() override;

  std::shared_ptr<Clipboard> CreateClipboard() override { return nullptr; }

  std::shared_ptr<FilePicker> CreateFilePicker() override;

  void SetMouseCursorStyle(SystemMouseCursor system_mouse_cursor) override;

 private:
  PlatformWeb();
  std::shared_ptr<ChannelRuntime> channel_runtime_;
  std::vector<std::shared_ptr<PlatformViewWeb>> overlay_view_pool_;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
