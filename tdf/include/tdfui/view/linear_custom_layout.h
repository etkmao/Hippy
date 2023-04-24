//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "tdfui/view/custom_layout_view.h"

namespace tdfcore {
/**
 * @brief 线性布局
 */
class LinearCustomLayout : public CustomLayout, public Object {
 public:
  ~LinearCustomLayout() override = default;
  /**
   * @brief 构造方法
   * @param orientation 方向（默认垂直方向）
   */
  explicit LinearCustomLayout(Orientation orientation = Orientation::kVertical);

 protected:
  void Layout(bool relayout) override;
  void OnOffsetUpdate(TPoint offset) override;
  TPoint GetPosition(int64_t index) override;
  // 处理上(左)滑
  void HandleScrollForward(TPoint offset);
  // 处理下(右)滑
  void HandleScrollReverse(TPoint offset);
  void HandleSticky(TPoint offset);
  void HandleStickyItemUpdate(const std::shared_ptr<CustomLayoutView> &custom_layout_view, int64_t index,
                              float scroll_distance);
  void DoLayout(const std::shared_ptr<CustomLayoutView> &custom_layout_view);
  /**
   * 重排版时获取第一个Item的布局信息，第一个item滚动了多少，重排版后要还原。
   * 独立成一个方法，降低代码复杂度。
   *
   * @return 原来第一个Item的 {索引，滚动距离，相对于自身大小的滚动比例}
   */
  std::tuple<int64_t, float, float> GetFirstItemInfo(const std::shared_ptr<CustomLayoutView> &custom_layout_view);
  void UpdateStickyItem(const std::shared_ptr<CustomLayoutView> &custom_layout_view, int64_t start_index);
  void UpdateContentRect(const std::shared_ptr<CustomLayoutView> &custom_layout_view, bool has_item);
  TPoint GetOffscreenItemPosition(int64_t index, const std::shared_ptr<CustomLayoutView> &custom_layout_view);
  TPoint GetOnScreenItemPosition(int64_t index, const std::shared_ptr<CustomLayoutView> &custom_layout_view);
  TPoint GetNewItemPosition(int64_t index, const std::shared_ptr<CustomLayoutView> &custom_layout_view);

  Orientation orientation_;
  TSize size_;
  int64_t item_count_ = 0;

  struct ItemData {
    int64_t index;
    std::shared_ptr<View> item;
  };

  // 当前屏幕内所有的item
  std::vector<ItemData> items_;
  // 正在吸顶的item
  ItemData sticking_item_;
  std::vector<ItemData> offscreen_sticky_items_;
};
}  // namespace tdfcore
