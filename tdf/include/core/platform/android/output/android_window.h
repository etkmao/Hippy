//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <android/native_window.h>
#include "core/common/size.h"
#include "core/common/object.h"

namespace tdfcore {

/**
 * @brief ANativeWindow管理类
 */
class AndroidWindow : public Object {
 public:
  using WindowRef = ANativeWindow*;

  explicit AndroidWindow(WindowRef window_ref);

  virtual ~AndroidWindow();

  /**
   * @brief window是否处于有效状态
   * @return
   */
  bool IsValid() const;

  /**
   * @brief 获取Window ref
   * @return WindowRef
   */
  WindowRef GetWindowRef() const;

  /**
   * @brief 获取Window的大小
   * @return window大小
   */
  TSize GetSize() const;

 private:
  WindowRef window_ref_ = nullptr;
};

}  // namespace tdfcore