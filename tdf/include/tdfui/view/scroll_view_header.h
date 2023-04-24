//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "tdfui/view/view.h"

namespace tdfcore {

class ScrollView;
/**
 * @brief 可以放置在ScrollView顶部的View
 */
class ScrollViewHeader : public Object, public std::enable_shared_from_this<ScrollViewHeader> {
 public:
  /**
   * @brief 子类必须实现，返回当前状态对应的View
   * @return
   */
  virtual std::shared_ptr<View> GetView() = 0;

 protected:
  void SetScrollView(const std::weak_ptr<ScrollView>& scroll_view);
  virtual void UpdateContentRect(const TRect& rect);
  /**
   * @brief ScrollView手势滑动的回调
   * @param old_offset 滑动之前的offset
   * @param new_offset 滑动之后的offset
   */
  virtual void OnScroll(const TPoint& old_offset, const TPoint& new_offset) {}
  /**
   * @brief ScrollView滑动手势释放回调
   */
  virtual void OnRelease() {}
  /*
   * @brief ScrollView物理动画结束回调
   */
  virtual void OnScrollEnd() {}

  std::weak_ptr<ScrollView> scroll_view_;
  friend class ScrollView;
};

}  // namespace tdfcore
