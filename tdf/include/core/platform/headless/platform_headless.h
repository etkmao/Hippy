//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/engine/graphics/device.h"
#include "core/engine/schedule/thread_task_driver.h"
#include "core/platform/common/platform.h"

namespace tdfcore {

class PlatformHeadless : public Platform {
 public:
  PlatformHeadless() : Platform() {}

  std::shared_ptr<TaskDriver> CreateVSyncTaskDriver() override { return TDF_MAKE_SHARED(ThreadTaskDriver); };

  std::shared_ptr<FileManager> GetFileManager() override { return nullptr; }

  bool IsMainThread() const override { return true; }

  std::string GetAppRootPath() const override { return ""; }

  std::string GetCacheDirectoryPath() const override { return ""; }

  float GetDeviceRatio() const override { return 1.0; }

  std::shared_ptr<PlatformEmbeddedView> CreateEmbeddedView(const std::string &view_type, int64_t view_id,
                                                           const PlatformEmbeddedView::PropsMap &propsMap) override {
    return nullptr;
  }

  std::shared_ptr<PlatformView> CreateOverlayView(int64_t view_id,
                                                  const std::shared_ptr<Scheduler> &scheduler) override {
    return nullptr;
  }

  std::shared_ptr<Keyboard> CreateKeyboard() override { return nullptr; }

  std::shared_ptr<Clipboard> CreateClipboard() override { return nullptr; }

  std::shared_ptr<FilePicker> CreateFilePicker() override { return nullptr; }

  void SetMouseCursorStyle(SystemMouseCursor system_mouse_cursor) override {}
};
}  // namespace tdfcore
