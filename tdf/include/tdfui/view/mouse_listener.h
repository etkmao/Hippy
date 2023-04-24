//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "tdfui/view/view.h"

namespace tdfcore {

/***
 * @brief 提供目标View的鼠标悬浮监听能力
 */
class MouseListener : public Object, public std::enable_shared_from_this<MouseListener> {
 public:
  enum class State {
    kNone,     /// 鼠标和目标View未发生交互
    kEntered,  /// 鼠标进入目标View区域
    kMove,     /// 鼠标在目标View区域移动
    kExited    /// 鼠标退出目标View区域
  };
  // 回调函数，position参数为全局坐标系位置
  using Callback = std::function<void(State state, const TPoint& position)>;
  /**
   * @brief 构造函数
   * @param view 监听目标View
   * @param callback 鼠标监听回调
   */
  MouseListener(const std::shared_ptr<View>& view, const Callback& callback);

  void Init() override;

 private:
  void UpdateMouseState(const TPoint& global_position);
  std::weak_ptr<View> target_view_;
  Callback callback_ = nullptr;
  bool current_mouse_in_view_ = false;
};

}  // namespace tdfcore