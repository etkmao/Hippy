//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "tdfui/view/custom_layout_view.h"

namespace tdfcore {
/**
 * @brief 瀑布流自定义布局
 */
class WaterFallCustomLayout : public CustomLayout {
 public:
  virtual ~WaterFallCustomLayout() = default;
  /**
   * @brief 构造方法
   * @param span_count 列(行)数
   * @param orientation 方向(默认纵向)
   */
  explicit WaterFallCustomLayout(int64_t span_count, Orientation orientation = Orientation::kVertical);

  /**
   * @brief 获取列(行)数
   * @return 列(行)数
   */
  constexpr int64_t GetSpanCount() const { return span_count_; }

  /**
   * @brief 设置列(行)数
   * @param count 列(行)数
   */
  void SetSpanCount(int64_t count) { span_count_ = count; }

  /**
   * @brief 获取每列(行)间距
   * 如果方向是`kVertical`就代表`列间距`
   * 如果方向是`kHorizontal`就代表`行间距`
   * @return 每列(行)间距
   */
  constexpr float GetLineSpacing() const { return line_spacing_; }

  /**
   * @brief 设置每列(行)间距
   * 如果方向是`kVertical`就代表`列间距`
   * 如果方向是`kHorizontal`就代表`行间距`
   * @param spacing 每列(行)间距
   */
  void SetLineSpacing(float spacing) { line_spacing_ = spacing; }

  /**
   * @brief 获取元素间距
   * @return 元素间距
   */
  constexpr float GetItemSpacing() const { return item_spacing_; }

  /**
   * @brief 设置元素间距
   * @param spacing 元素间距
   */
  void SetItemSpacing(float spacing) { item_spacing_ = spacing; }

 protected:
  void Layout(bool relayout = false) override;
  void OnOffsetUpdate(TPoint offset) override;
  TPoint GetPosition(int64_t index) override;
  void DoLayout(const std::shared_ptr<CustomLayoutView> &custom_layout_view);
  std::vector<float> PrepareLayout(const std::shared_ptr<CustomLayoutView> &custom_layout_view,
                                   int64_t start_changing_index, int64_t start_displaying_index);
  void HandleScrollForward(TPoint offset);
  void HandleScrollReverse(TPoint offset);
  void HandleScrollReverseAdding(const std::shared_ptr<CustomLayoutView> &custom_layout_view, float scroll_distance);
  void HandleScrollReverseRemoving(const std::shared_ptr<CustomLayoutView> &custom_layout_view, float scroll_distance);
  void UpdateContentRect(const std::shared_ptr<CustomLayoutView> &custom_layout_view, bool has_item);
  TPoint GetOffscreenItemPosition(int64_t index, const std::shared_ptr<CustomLayoutView> &custom_layout_view);
  TPoint GetOnScreenItemPosition(int64_t index, const std::shared_ptr<CustomLayoutView> &custom_layout_view);
  TPoint GetNewItemPosition(int64_t index, const std::shared_ptr<CustomLayoutView> &custom_layout_view);
  int64_t GetIndexOfMinimumValue(const std::vector<float> &vector);
  int64_t GetLongestSpan();
  int64_t GetShortestSpan();
  int64_t GetFirstIndex();
  int64_t GetLastIndex();
  bool IsItemsMapEmpty() const;

  Orientation orientation_;
  TSize size_;
  int64_t span_count_ = 0;
  int64_t item_count_ = 0;
  float item_width_ = 0;
  float line_spacing_ = 0;
  float item_spacing_ = 0;

  struct ItemData {
    int64_t index;
    std::shared_ptr<View> item;
  };

  struct ItemFrameData {
    int64_t index = 0;
    TRect data;
  };

  struct OffScreenItemData {
    int64_t index;
    double top;
    double bottom;
  };

  std::unordered_map<int64_t, std::vector<ItemData>> items_map_;
  std::unordered_map<int64_t, std::vector<OffScreenItemData>> off_screen_items_map_;
};
}  // namespace tdfcore
