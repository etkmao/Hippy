//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "tdfui/view/navigation_bar.h"
#include "tdfui/view/tab_bar.h"

namespace tdfcore {
class Scaffold : public View {
  TDF_REFF_CLASS_META(Scaffold)

 public:
  /**
   * @brief 初始化
   */
  void Init() override;

  /**
   * @brief 设置标题
   * @param title 标题
   */
  void SetTitle(const std::u16string &title);

  /**
   * @brief 获取标题
   * @return 标题
   */
  std::u16string GetTitle() const;

  /**
   * @brief 设置TabBar项目
   * @param items 项目集合
   */
  void SetTabBarItems(const std::vector<TabBarItem> &items);

  /**
   * @brief 获取TabBar项目集合
   * @return TabBar项目集合
   */
  std::vector<TabBarItem> GetTabBarItems() const;

  /**
   * @brief 设置是否隐藏NavigationBar
   * @param is_hidden 是否隐藏
   */
  void SetNavigationBarHidden(bool is_hidden);

  /**
   * @brief 设置是否隐藏TabBar
   * @param is_hidden 是否隐藏
   */
  void SetTabBarHidden(bool is_hidden);

  /**
   * @brief 获取NavigationBar是否隐藏
   * @return 是否隐藏NavigationBar
   */
  constexpr bool GetNavigationBarIsHidden() const { return navigation_bar_is_hidden_; }

  /**
   * @brief 获取TabBar是否隐藏
   * @return 是否隐藏TabBar
   */
  constexpr bool GetTabBarIsHidden() const { return tab_bar_is_hidden_; }

  /**
   * @brief 添加View到内容视图
   * @param view 子视图
   */
  void AddViewToContentView(const std::shared_ptr<View> &view);

  /**
   * @brief 移除内容视图的View
   * @param view 子视图
   */
  void RemoveViewFromContentView(const std::shared_ptr<View> &view);

  /**
   * @brief 获取 navigation bar
   */
  std::shared_ptr<NavigationBar> GetNavigationBar() const { return navigation_bar_; }

  /**
   * @brief 获取 tab bar
   */
  std::shared_ptr<TabBar> GetTabBar() const { return tab_bar_; }

 protected:
  explicit Scaffold(bool navigation_bar_hidden = false, bool tab_bar_hidden = true,
                    const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent());
  virtual void OnTabBarClick(int64_t index);
  void Mount() override;
  void Layout() override;

  bool navigation_bar_is_hidden_ = false;
  bool tab_bar_is_hidden_ = true;
  std::shared_ptr<NavigationBar> navigation_bar_;
  std::shared_ptr<TabBar> tab_bar_;
  std::shared_ptr<View> content_view_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::Scaffold, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::Scaffold)
