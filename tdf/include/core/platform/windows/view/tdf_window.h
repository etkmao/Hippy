//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include "core/common/object.h"
#include "core/common/rect.h"
#include "core/platform/windows/windows_define.h"

namespace tdfcore {
using MessageHandler = std::function<LRESULT(UINT const, WPARAM const, LPARAM const)>;
class TDFView;
class TDFWindow : public Object {
 public:
  /***
   * @brief 构造函数
   * @param message_handler Window消息处理函数
   */
  TDFWindow() = default;
  explicit TDFWindow(const MessageHandler& message_handler);
  ~TDFWindow();

  /***
   * @brief 初始化Window
   * @param Rect 窗口位置大小
   */
  virtual void Initialize(const std::string_view& title, const TRect& frame,
                          DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

  /***
   * @brief 返回window handle
   * @return
   */
  HWND GetWindowHandle() const { return window_handle_; }

  /***
   * @brief 显示窗口
   */
  void Show();

  /***
   * @brief 隐藏窗口
   */
  void Hide();

  /***
   * @brief 添加子窗口
   * @param child_handle
   */
  void AddChild(const std::shared_ptr<TDFWindow>& child_window);

  /***
   * @brief 添加View到窗口上
   * @param view
   */
  void AddView(TDFView* view);

  /***
   * @brief 移除子窗口
   * @param child_handle
   */
  void RemoveChild(const std::shared_ptr<TDFWindow>& child_window);

  /***
   * @brief 移除窗口上的View
   * @param view
   */
  void RemoveView(TDFView* view);

 private:
  /**
   * @brief 窗口过程函数，处理WM_NCCREATE消息，其余尝试交给MessageHandler处理
   */
  static LRESULT CALLBACK WndProc(HWND const window, UINT const message, WPARAM const wparam,
                                  LPARAM const lparam) noexcept;
  virtual LRESULT MessageHandle(UINT const, WPARAM const, LPARAM const);
  void Destroy();

  MessageHandler message_handler_ = nullptr;
  HWND window_handle_ = nullptr;
  std::wstring window_class_name_;
};
}  // namespace tdfcore