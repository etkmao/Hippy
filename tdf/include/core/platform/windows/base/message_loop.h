//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include "core/common/object.h"
#include "core/platform/windows/windows_define.h"

#define TDF_WM_EXECUTE_FUNCTION (WM_USER + 1)

namespace tdfcore {

/**
 * @brief 自定义的窗口消息及操作
 */
class MessageLoop : public Object {
 public:
  FRIEND_OF_TDF_ALLOC;

  static void SendExecuteFunction(HWND window_handle, std::function<void()> func);
  void ExecuteFunction();

 private:
  std::function<void()> executable_function_;
};

}  // namespace tdfcore
