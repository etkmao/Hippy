//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "main_window.h"

#include <mutex>

#include "config.h"
#include "demo_window.h"

class MainWindow : public std::enable_shared_from_this<MainWindow> {
 public:
  MainWindow() = default;
  void Show();
  static LRESULT CALLBACK WindowProc(HWND const window, UINT const message, WPARAM const wparam,
                                     LPARAM const lparam) noexcept;

 private:
  LRESULT CALLBACK HandleMassage(HWND const window, UINT const message, WPARAM const wparam,
                                 LPARAM const lparam) noexcept;
  void Destroy();
  void OpenDemoWindow();
  void CloseDemoWindow();

 private:
  HWND window_handle_{nullptr};
  HWND open_button_handle_{nullptr};
  HWND close_button_handle_{nullptr};
  std::shared_ptr<DemoWindow> demo_window_{nullptr};
  std::shared_ptr<hippy::windows::framework::Config> config_{nullptr};
  std::mutex mutex_;
};
