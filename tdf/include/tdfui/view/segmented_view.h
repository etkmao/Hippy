//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "tdfui/view/button.h"
#include "tdfui/view/label.h"
namespace tdfcore {
struct SegmentedItem {
  std::u16string title;
  std::shared_ptr<Image> image;
};

class SegmentedView : public View {
  TDF_REFF_CLASS_META(SegmentedView)

 public:
  void Init() override;

  /**
   * @brief 设置项目列表
   * title和image只能取其一，不能同时生效
   * @param items 项目列表
   */
  void SetItems(const std::vector<SegmentedItem> &items);

  /**
   * @brief 获取项目列表
   * @return 项目列表
   */
  constexpr const std::vector<SegmentedItem> &GetItems() const { return items_; }

  /**
   * @brief 设置选中项目
   * @param index 选中项目索引
   */
  void SetSelectedIndex(int64_t index);

  /**
   * @brief 获取选中项目索引
   * @return 选中项目索引
   */
  constexpr int64_t GetSelectedIndex() const { return selected_index_; }

  /**
   * @brief 设置主题色
   * @param color 主题色
   */
  void SetTintColor(Color color);

  /**
   * @brief 获取主题色
   * @return 主题色
   */
  constexpr Color GetTintColor() const { return tint_color_; }

  /**
   * @brief 设置主题文本色
   * @param color 主题文本色
   */
  void SetTintTitleColor(Color color);

  /**
   * @brief 获取主题文本色
   * @return 主题文本色
   */
  constexpr Color GetTintTitleColor() const { return tint_title_color_; }

  /**
   * @brief 设置点击监听器
   * @param listener 点击监听器
   */
  void SetTapListener(const std::function<void(int64_t)> &listener);

  /**
   * @brief 获取点击监听器
   * @return 点击监听器
   */
  constexpr const std::function<void(int64_t)> &GetTapListener() const { return listener_; }

 protected:
  explicit SegmentedView(const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent());
  void Layout() override;
  void SetNormal(const std::shared_ptr<Button> &button);
  void SetSelected(const std::shared_ptr<Button> &button);

  Color tint_color_;
  Color tint_title_color_;
  int64_t selected_index_;
  std::function<void(int64_t)> listener_;
  std::vector<SegmentedItem> items_;
  std::vector<std::shared_ptr<Button>> buttons_;
  std::vector<std::shared_ptr<View>> lines_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::SegmentedView, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::SegmentedView)
