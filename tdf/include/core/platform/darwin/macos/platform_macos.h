//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#import <MetalKit/MetalKit.h>
#include "core/platform/common/platform.h"
#include "core/platform/common/view/platform_view.h"
#include "core/platform/darwin/common/EmbeddedViewRegister.h"

namespace tdfcore {
class PlatformMacOS final : public Platform {
 public:
  explicit PlatformMacOS();

  ~PlatformMacOS();

  std::shared_ptr<TaskDriver> CreateVSyncTaskDriver() override;

  std::shared_ptr<TaskDriver> CreateRunLoopTaskDriver() override;

  std::shared_ptr<PlatformEmbeddedView> CreateEmbeddedView(const std::string &view_type, int64_t view_id,
                                                           const PlatformEmbeddedView::PropsMap &propsMap) override;

  std::shared_ptr<PlatformView> CreateOverlayView(int64_t view_id,
                                                  const std::shared_ptr<Scheduler> &scheduler) override;

  std::shared_ptr<ChannelRuntime> GetChannelRuntime() override;

  std::shared_ptr<PlatformWindow> MakeWindow(const std::string_view &title, const TRect &frame) override;

  void SetEmbeddedViewRegister(EmbeddedViewRegister *embeddedViewRegister) {
    embeddedViewRegister_ = embeddedViewRegister;
  }
  void SetMouseCursorStyle(SystemMouseCursor system_mouse_cursor) override;

 private:
  bool IsMainThread() const override;
  std::shared_ptr<Keyboard> CreateKeyboard() override;
  std::shared_ptr<Clipboard> CreateClipboard() override;
  std::shared_ptr<FilePicker> CreateFilePicker() override;
  std::string GetAppRootPath() const override;
  std::string GetCacheDirectoryPath() const override;
  std::shared_ptr<FileManager> GetFileManager() override;
  std::shared_ptr<Menu> GetMenu() override;
  float GetDeviceRatio() const override;

  std::shared_ptr<FileManager> file_manager_;
  std::shared_ptr<Menu> menu_;
  std::shared_ptr<ChannelRuntime> channel_runtime_;
  EmbeddedViewRegister *embeddedViewRegister_;

  std::unordered_map<SystemMouseCursor, NSCursor *> system_cursors_;
  void HideMouseCursor();
  void UnHideMouseCursor();
  BOOL mouse_cursor_hidden_ = NO;
};

}  // namespace tdfcore
