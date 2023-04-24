//
// Copyright (c) 2021 The tencent Authors. All rights reserved.
//

#pragma once

#include "core/common/object.h"
#include "core/tdfi/shell.h"

namespace tdfcore {
/*
 * 鼠标状态管理类
 */
class MouseManager : public Object, public std::enable_shared_from_this<MouseManager>  {
 public:
  ~MouseManager() override;
  /**
   * @brief 获取当前的上下文
   * @return 指向当前上下文的指针
   */
  static std::shared_ptr<MouseManager> GetCurrent();
  /**
   * @brief 设置为当前的上下文
   */
  void SetCurrent();

  /**
   * @brief 设置全局鼠标悬浮指针类型
   * @param Cursor 悬浮指针类型
   */
  void SetGlobalCursor(const SystemMouseCursor &cursor);

  /**
   * @brief 取消全局鼠标悬浮指针类型
   */
  void ResetGlobalCursor();

  /**
   * @brief 更新当前指针，默认鼠标事件、build事件触发时调用。支持手工调用
   */
  void UpdateCursor();

  /**
   * @brief 获取鼠标悬浮指针类型
   * @param Cursor 悬浮指针类型
   */
  SystemMouseCursor GetGlobalCursor() const { return global_cursor_; }

 protected:
  explicit MouseManager(std::shared_ptr<Shell> shell);
  void Init() override;

 private:
  std::shared_ptr<Shell> shell_;
  TPoint mouse_point_;
  bool need_global_cursor_ = false;
  SystemMouseCursor global_cursor_ = SystemMouseCursor::kBasic;
  SystemMouseCursor current_view_cursor = SystemMouseCursor::kBasic;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore