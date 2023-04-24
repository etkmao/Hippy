//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include "core/common/object.h"
#include "core/common/rect.h"
namespace tdfcore {

class PlatformView;
class PlatformWindow : public Object {
 public:
  /***
   * @brief 构造函数
   * @param title 窗口标题
   * @param frame 窗口位置和大小
   */
  PlatformWindow(const std::string_view& title, const TRect& frame) : title_(title), frame_(frame) {}

  /**
   * @brief 弹出窗口
   */
  virtual void Show() = 0;

  /**
   * @brief 隐藏/关闭窗口
   */
  virtual void Hide() = 0;

  /**
   * @brief 窗口增加一个TDFView
   * @param platform_view
   */
  virtual void AddView(const std::shared_ptr<PlatformView>& platform_view) = 0;

  /**
   * @brief 窗口移除一个TDFView
   * @param platform_view
   */
  virtual void RemoveView(const std::shared_ptr<PlatformView>& platform_view) = 0;

 protected:
  std::string title_;
  TRect frame_;
};

}  // namespace tdfcore
