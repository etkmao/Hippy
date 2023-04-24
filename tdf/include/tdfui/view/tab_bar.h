//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <unordered_map>
#include "tdfui/view/button.h"
#include "tdfui/view/image_view.h"
#include "tdfui/view/label.h"
namespace tdfcore {
static constexpr float kDefaultFontSize = 12.0;
struct TabBarItem {
 public:
  std::u16string title;
  std::u16string selected_title;
  std::shared_ptr<Image> image;
  std::shared_ptr<Image> selected_image;
  std::shared_ptr<View> view;

  TabBarItem(const std::u16string &title, const std::u16string &selected_title, const std::shared_ptr<Image> &image,
             const std::shared_ptr<Image> &selected_image, const std::shared_ptr<View> &view)
      : title(title), selected_title(selected_title), image(image), selected_image(selected_image), view(view) {}
};

class TabBar : public View {
  TDF_REFF_CLASS_META(TabBar)

 public:
  /**
   * @brief 初始化
   */
  void Init() override;

  /**
   * @brief 设置项目集合
   * @param items 项目集合
   */
  void SetItems(const std::vector<TabBarItem> &items);

  /**
   * @brief 添加一个Tab项目
   * @param item 项目集合
   */
  void AddItem(const TabBarItem &item);

  /**
   * @brief 获取项目集合
   * @return 项目集合
   */
  constexpr const std::vector<TabBarItem> &GetItems() const { return items_; }

  /**
   * @brief 设置点击回调
   * @param listener 点击回调
   */
  void SetClickListener(const std::function<void(int64_t index)> &listener);

  /**
   * @brief 设置选中字体颜色
   * @param color 颜色
   */
  void SetSelectedTitleColor(Color color);

  /**
   * @brief 获取选中字体颜色
   * @return 颜色
   */
  constexpr Color GetSelectedTitleColor() const { return selected_title_color_; }

  /**
   * @brief 设置普通字体颜色
   * @param color 颜色
   */
  void SetNormalTitleColor(Color color);

  /**
   * @brief 获取谱图字体颜色
   * @return 颜色
   */
  constexpr Color GetNormalTitleColor() const { return normal_title_color_; }

  /**
   * @brief 设置选中背景颜色
   * @param color 颜色
   */
  void SetSelectedBackgroundColor(Color color);

  /**
   * @brief 获取选中背景颜色
   * @return 颜色
   */
  constexpr Color GetSelectedBackgroundColor() const { return selected_background_color_; }

  /**
   * @brief 设置普通背景颜色
   * @param color 颜色
   */
  void SetNormalBackgroundColor(Color color);

  /**
   * @brief 获取普通背景颜色
   * @return 颜色
   */
  constexpr Color GetNormalBackgroundColor() const { return normal_background_color_; }

  /**
   * @brief 获取点击回调
   * @return 点击回调
   */
  constexpr const std::function<void(int64_t index)> &GetClickListener() const { return click_listener_; }

  void SetFontSize(float font_size);

 protected:
  explicit TabBar(const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent());
  void Layout() override;
  std::shared_ptr<Button> CreateButton(int64_t index);
  void SetSelected(const std::shared_ptr<Button> &button, const TabBarItem &item);
  void SetNormal(const std::shared_ptr<Button> &button, const TabBarItem &item);

  Color normal_title_color_;
  Color selected_title_color_;
  Color normal_background_color_;
  Color selected_background_color_;
  int64_t selected_index_ = 0;
  std::function<void(int64_t index)> click_listener_;
  std::vector<TabBarItem> items_;
  std::vector<std::shared_ptr<Button>> buttons_;
  float font_size_ = kDefaultFontSize;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::TabBar, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::TabBar)
