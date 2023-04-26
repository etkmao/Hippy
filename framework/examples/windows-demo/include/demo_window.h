//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/platform/windows/tdf_engine_windows.h"
#include "core/platform/windows/view/tdf_window.h"
#include "renderer/tdf/tdf_render_manager.h"
#include "tdfui/view/view_context.h"
#include "framework.h"

class DemoWindow : public tdfcore::TDFWindow, public std::enable_shared_from_this<DemoWindow> {
 public:
  DemoWindow();
  ~DemoWindow() override;

  void Initialize(const std::string_view& title, const tdfcore::TRect& frame,
                  DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE) override;
  void SetFramework(std::shared_ptr<hippy::windows::Framework>& framework) { framework_ = framework; }
  void SetDismissHandler(const std::function<void()>& handler) { dismiss_handler_ = handler; }


 protected:
  LRESULT MessageHandle(UINT const message, WPARAM const wparam, LPARAM const lparam) override;
  void SetupDemoApp();

 public:
  std::shared_ptr<tdfcore::TDFEngineWindows> tdf_engine_;
  std::shared_ptr<hippy::windows::Framework> framework_;
  std::function<void()> dismiss_handler_;
};
