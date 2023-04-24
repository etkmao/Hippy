//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "tdfui/view/button.h"
#include "tdfui/view/label.h"

namespace tdfcore {

extern const std::u16string kDefaultBackButtonAccessibilityLabel;

class NavigationBar : public View {
  TDF_REFF_CLASS_META(NavigationBar)

 public:
  /**
   * @brief 初始化
   */
  void Init() override;

  /**
   * @brief 设置是否展示返回按钮
   * @param should_show_back_button 是否展示
   */
  void SetShowBackButton(bool should_show_back_button);

  /**
   * @brief 设置标题
   * @param title 标题
   */
  void SetTitle(const std::u16string &title);

  /**
   * @brief 获取标题
   * @return 标题
   */
  constexpr const std::u16string &GetTitle() const { return title_; }

  /**
   * @brief 设置自定义标题视图
   * @param title_view 自定义标题视图
   */
  void SetTitleView(std::shared_ptr<View> title_view);

  /**
   * @brief 获取标题视图
   * @return 标题视图
   */
  constexpr const std::shared_ptr<View> &GetTitleView() const { return title_view_; }

  /**
   * @brief 自定义返回Image
   * @param back_image 返回image
   */
  void SetBackImage(std::shared_ptr<Image> back_image);

  /**
   * @brief 获取返回图标
   * @return 返回图标
   */
  constexpr const std::shared_ptr<Image> &GetBackImage() const { return back_image_; }

  /**
   * @brief 自定义返回按钮的AccessibilityLabel
   */
  void SetBackButtonAccessibilityLabel(const std::u16string &label) { back_button_->SetAccessibilityLabel(label); }

  /**
   * @brief 获取返回按钮的AccessibilityLabel
   */
  const std::u16string &GetBackButtonAccessibilityLabel() const {
    return back_button_->GetAccessibilityLabel();
  }

  /**
   * @brief 自定义左Item视图
   * @param left_item_view 左Item视图
   */
  void SetLeftItemView(std::shared_ptr<View> left_item_view);

  /**
   * @brief 获取左Item视图
   * @return 左Item视图
   */
  constexpr const std::shared_ptr<View> &GetLeftItemView() const { return left_item_view_; }

  /**
   * @brief 自定义右边Item视图
   * @param right_item_view 右边视图
   */
  void SetRightItemView(std::shared_ptr<View> right_item_view);

  /**
   * @brief 获取右Item视图
   * @return 右Item视图
   */
  constexpr const std::shared_ptr<View> &GetRightItemView() const { return right_item_view_; }

  /**
   * @brief 获取返回事件监听者
   * @param listener 返回事件监听者
   */
  void SetBackActionListener(std::function<void()> listener);

  /**
   * @brief 获取返回事件监听者
   * @return 返回事件监听者
   */
  std::function<void()> GetBackActionListener() const;

  /**
   * @brief 获取 title label
   */
  std::shared_ptr<Label> GetTitleLabel() const { return title_label_; }

 protected:
  explicit NavigationBar(const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent())
      : View(view_context) {}
  void Layout() override;

  void Mount() override;

  std::u16string title_;
  std::shared_ptr<Image> back_image_;
  std::function<void()> back_action_listener_;
  bool should_show_back_button_ = true;
  std::shared_ptr<View> title_view_;
  std::shared_ptr<View> left_item_view_;
  std::shared_ptr<View> right_item_view_;
  std::shared_ptr<Label> title_label_;
  std::shared_ptr<Button> back_button_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::NavigationBar, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::NavigationBar)
