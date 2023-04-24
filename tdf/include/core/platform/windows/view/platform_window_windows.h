//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include "core/platform/common/platform_window.h"
#include "core/platform/windows/windows_define.h"

namespace tdfcore {

class TDFWindow;
class PlatformView;

class PlatformWindowWindows : public PlatformWindow,
                                      public std::enable_shared_from_this<PlatformWindowWindows> {
 public:
  PlatformWindowWindows(const std::string_view& title, const TRect& frame);
  void Init() override;
  void Show() override;
  void Hide() override;
  void AddView(const std::shared_ptr<PlatformView>& platform_view) override;
  void RemoveView(const std::shared_ptr<PlatformView>& platform_view) override;

 private:
  LRESULT MessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam);
  std::shared_ptr<TDFWindow> window_;
};

}  // namespace tdfcore
