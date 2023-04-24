//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <atomic>
#include <thread>
#include "core/common/macro.h"
#include "core/common/time.h"
#include "core/platform/common/vsync_task_driver.h"
#include "core/platform/windows/windows_define.h"

namespace tdfcore {

class VSyncTaskDriverWindows final : public VSyncTaskDriver {
 public:
  VSyncTaskDriverWindows(const Callback& callback, TDuration interval);
  ~VSyncTaskDriverWindows() override;

 protected:
  void OnStart() override;
  void OnStop() override;

 private:
  WNDCLASS RegisterWindowClass();
  LRESULT HandleMessage(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;
  static LRESULT CALLBACK WndProc(HWND const window, UINT const message, WPARAM const wparam,
                                  LPARAM const lparam) noexcept;
  HWND window_handle_;
  std::wstring window_class_name_;
  TDuration interval_;
  DWORD thread_id_ = 0;

  TDF_DISALLOW_COPY_AND_ASSIGN(VSyncTaskDriverWindows);
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore