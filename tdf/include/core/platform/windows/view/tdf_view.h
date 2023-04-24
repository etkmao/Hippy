//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/macro.h"
#include "core/common/object.h"
#include "core/common/rect.h"
#include "core/platform/windows/windows_define.h"

namespace tdfcore {
/**
 * @brief 用于管理窗口的尺寸和子窗口。
 */
class TDFView : public Object, public std::enable_shared_from_this<TDFView> {
 public:
  TDFView() = default;

  /**
   * @brief 获取平台window的句柄
   */
  virtual HWND GetWindowHandle() const { return window_handle_; }

  /**
   * @brief 获取frame
   */
  const TRect GetFrame() const;

  /**
   * @brief 设置frame
   */
  void SetFrame(const TRect& frame);

  /**
   * @brief 获取当前设备像素密度
   */
  double GetScaleFactor() const { return scale_factor_; }

  void SetScaleFactor(const double& scale_factor) { scale_factor_ = scale_factor; }

  /**
   * @brief 将View实例插入指定位置
   */
  virtual void InsertView(TDFView* view, int64_t index);

  /**
   * @brief 移除指定的view
   */
  virtual void RemoveView(TDFView* view);

 protected:
  virtual LRESULT MessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) { return 0; }
  double scale_factor_ = 1.0;
  HWND window_handle_;
  bool is_overlay_view_ = false;

  /**
   * @brief 返回子窗口数量
   */
  const int SubviewsCount() const;

  /**
   * @brief 添加子窗口(按win SetParent函数顺序)
   */
  virtual void addChild(TDFView* child_view);
};
}  // namespace tdfcore
