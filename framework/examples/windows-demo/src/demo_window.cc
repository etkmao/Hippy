//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#include "demo_window.h"

#include <Windows.h>
#include <filesystem>
#include <optional>

#include "core/platform/common/tdf_engine.h"

#define ID_MENU_OPERATOR 9001
#define ID_MENU_REFRESH 9002
#define ID_MENU_ABOUT 9003

using namespace tdfcore;

DemoWindow::DemoWindow()
    : tdf_engine_(TDF_MAKE_SHARED(TDFEngineWindows,
                                  TDFEngineConfig{.thread_mode_ = TDFEngineConfig::ThreadModel::Performance})) {}

DemoWindow::~DemoWindow() {}

void DemoWindow::Initialize(const std::string_view& title, const TRect& frame, DWORD style) {
  TDFWindow::Initialize(title, frame, style);
  tdf_engine_->GetOutputView()->SetFrame(TRect::MakeWH(frame.Width(), frame.Height()));
  tdf_engine_->Start();
  AddView(tdf_engine_->GetOutputView().get());
  ShowWindow(tdf_engine_->GetOutputView()->GetWindowHandle(), SW_SHOW);

  HMENU menu = CreateMenu();
  HMENU sub_menu = CreatePopupMenu();
  AppendMenu(sub_menu, MF_STRING, ID_MENU_ABOUT, "About");
  AppendMenu(sub_menu, MF_STRING, ID_MENU_REFRESH, "Refresh");
  AppendMenu(menu, MF_STRING | MF_POPUP, (UINT_PTR)sub_menu, "Operator");
  SetMenu(GetWindowHandle(), menu);
}

LRESULT DemoWindow::MessageHandle(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept {
  switch (message) {
    case WM_CREATE: {
      auto shell = tdf_engine_->GetShell();
      auto config = framework_->CreateDefaultConfig();
      config->SetShell(shell);
      framework_->Initialize(config);
      return 0;
    }
    case WM_COMMAND: {
      switch (LOWORD(wparam)) {
        case ID_MENU_REFRESH:
          framework_->Reload();
          break;
        case ID_MENU_ABOUT:
          MessageBox(GetWindowHandle(), "This is a sample example for hippy windows.", "About", MB_OK);
          break;
      }
      return 0;
    }
    case WM_SIZE: {
      int64_t width = LOWORD(lparam);
      int64_t height = HIWORD(lparam);
      tdf_engine_->GetOutputView()->SetFrame(TRect::MakeWH(width, height));
      return 0;
    }
    case WM_DESTROY: {
      auto result = DefWindowProc(GetWindowHandle(), message, wparam, lparam);
      if (dismiss_handler_) {
        dismiss_handler_();
      }
      return result;
    }
    default:
      return DefWindowProc(GetWindowHandle(), message, wparam, lparam);
  }
}
