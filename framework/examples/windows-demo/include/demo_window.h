//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <functional>

#include "config.h"
#include "core/platform/windows/tdf_engine_windows.h"
#include "core/platform/windows/view/tdf_window.h"
#include "framework.h"
#include "renderer/tdf/tdf_render_manager.h"
#include "tdfui/view/view_context.h"

class DemoWindow : public tdfcore::TDFWindow, public std::enable_shared_from_this<DemoWindow> {
 public:
  DemoWindow();
  ~DemoWindow() override;

  void Initialize(const std::string_view& title, const tdfcore::TRect& frame,
                  DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE) override;
  void SetFramework(const std::shared_ptr<hippy::windows::Framework>& framework) { framework_ = framework; }
  void SetConfig(const std::shared_ptr<hippy::windows::Config>& config) { config_ = config; }
  void SetDismissHandler(const std::function<void()>& handler) { dismiss_handler_ = handler; }
  void Destroy(std::function<void()> callback);

 protected:
  LRESULT MessageHandle(UINT const message, WPARAM const wparam, LPARAM const lparam) override;
  void SetupDemoApp();

 public:
  std::shared_ptr<tdfcore::TDFEngineWindows> tdf_engine_;
  std::shared_ptr<hippy::windows::Framework> framework_;
  std::shared_ptr<hippy::Config> config_;
  std::function<void()> dismiss_handler_;
};
