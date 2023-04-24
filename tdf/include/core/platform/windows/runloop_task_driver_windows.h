//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#include <memory>
#include "core/engine/schedule/task_driver.h"
#include "core/platform/windows/windows_define.h"

namespace tdfcore {

class RunLoopTaskDriverWindows : public TaskDriver {
 public:
  RunLoopTaskDriverWindows();
  ~RunLoopTaskDriverWindows() override;
  void WakeUp() override;

 protected:
  void OnStop() override;

 private:
  void ProcessTasks();
  WNDCLASS RegisterWindowClass();
  LRESULT HandleMessage(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;
  static LRESULT CALLBACK WndProc(HWND const window, UINT const message, WPARAM const wparam,
                                  LPARAM const lparam) noexcept;
  HWND window_handle_;
  std::wstring window_class_name_;
  DWORD thread_id_ = 0;
};

}  // namespace tdfcore