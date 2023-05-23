//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#include "main_window.h"

#include <Windows.h>
#include <functional>

#include "footstone/macros.h"
#include "framework.h"

#define OPEN_BUTTON_ID 5001
#define CLOSE_BUTTON_ID 5002
#define MAIN_WINDOW_CLASS "main_window"
#define MAIN_WINDOW_TITLE "Hippy"
#define BUTTON_WINDOW_CLASS "button"
#define OPEN_BUTTON_TITLE "Open Hippy Demo"
#define CLOSE_BUTTON_TITLE "Close Hippy Demo"

using namespace tdfcore;

void MainWindow::Show() {
  WNDCLASS window_class{};
  window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
  window_class.lpszClassName = MAIN_WINDOW_CLASS;
  window_class.style = CS_HREDRAW | CS_VREDRAW;
  window_class.cbClsExtra = 0;
  window_class.cbWndExtra = 0;
  window_class.hInstance = GetModuleHandle(nullptr);
  window_class.hIcon = LoadIcon(window_class.hInstance, MAKEINTRESOURCE(101));
  window_class.hbrBackground = 0;
  window_class.lpszMenuName = nullptr;
  window_class.lpfnWndProc = MainWindow::WindowProc;
  RegisterClass(&window_class);
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  auto window_style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
  window_handle_ = CreateWindow(MAIN_WINDOW_CLASS, MAIN_WINDOW_TITLE, window_style, 0, 0, 400, 400, nullptr, nullptr,
                                GetModuleHandle(nullptr), this);
  open_button_handle_ = CreateWindow(BUTTON_WINDOW_CLASS, OPEN_BUTTON_TITLE, WS_CHILD | WS_VISIBLE, 130, 100, 140, 40,
                                     window_handle_, (HMENU)OPEN_BUTTON_ID, GetModuleHandle(nullptr), nullptr);
  close_button_handle_ = CreateWindow(BUTTON_WINDOW_CLASS, CLOSE_BUTTON_TITLE, WS_CHILD | WS_VISIBLE, 130, 180, 140, 40,
                                      window_handle_, (HMENU)CLOSE_BUTTON_ID, GetModuleHandle(nullptr), nullptr);
  config_ = hippy::windows::Framework::CreateDefaultConfig();
}

LRESULT CALLBACK MainWindow::WindowProc(HWND const window, UINT const message, WPARAM const wparam,
                                        LPARAM const lparam) noexcept {
  if (message == WM_NCCREATE) {
    auto window_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
    SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window_struct->lpCreateParams));
    return DefWindowProc(window, message, wparam, lparam);
  }
  auto main_window = reinterpret_cast<MainWindow*>(GetWindowLongPtr(window, GWLP_USERDATA));
  return main_window->HandleMassage(window, message, wparam, lparam);
}

LRESULT CALLBACK MainWindow::HandleMassage(HWND const window, UINT const message, WPARAM const wparam,
                                           LPARAM const lparam) noexcept {
  switch (message) {
    case WM_COMMAND:
      if (LOWORD(wparam) == OPEN_BUTTON_ID && HIWORD(wparam) == BN_CLICKED) {
        auto main_window = reinterpret_cast<MainWindow*>(GetWindowLongPtr(window, GWLP_USERDATA));
        main_window->OpenDemoWindow();
        return 0;
      } else if (LOWORD(wparam) == CLOSE_BUTTON_ID && HIWORD(wparam) == BN_CLICKED) {
        auto main_window = reinterpret_cast<MainWindow*>(GetWindowLongPtr(window, GWLP_USERDATA));
        main_window->CloseDemoWindow();
        return 0;
      }
      break;
    case WM_DESTROY:
      window_handle_ = nullptr;
      Destroy();
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProc(window, message, wparam, lparam);
}

void MainWindow::Destroy() {
  if (window_handle_) {
    DestroyWindow(window_handle_);
    window_handle_ = nullptr;
  }
  UnregisterClass(MAIN_WINDOW_CLASS, nullptr);
}

void MainWindow::OpenDemoWindow() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (demo_window_) {
    return;
  }
  std::shared_ptr<hippy::Framework> framework = std::make_shared<hippy::Framework>();
  demo_window_ = std::make_shared<DemoWindow>();
  demo_window_->SetFramework(framework);
  demo_window_->SetConfig(config_);
  demo_window_->Initialize("Hippy Window Demo", tdfcore::TRect::MakeXYWH(0, 0, 600, 800));
  demo_window_->Show();
}

void MainWindow::CloseDemoWindow() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (demo_window_) {
    std::function<void()> destroy_callback = [WEAK_THIS]() {
      DEFINE_AND_CHECK_SELF(MainWindow)
      self->demo_window_->SetFramework(nullptr);
      self->demo_window_->SetConfig(nullptr);
      SendMessage(self->demo_window_->GetWindowHandle(), WM_CLOSE, 0, 0);
      self->demo_window_ = nullptr;
    };
    demo_window_->Destroy(destroy_callback);
  }
}
