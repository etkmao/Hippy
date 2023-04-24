//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <optional>
#include <unordered_map>
#include "core/support/gesture/recognizer/long_press_gesture_recognizer.h"
#include "core/support/gesture/recognizer/tap_gesture_recognizer.h"
#include "core/support/gesture/recognizer/hover_gesture_recognizer.h"
#include "tdfui/view/edge_insets.h"
#include "tdfui/view/image_view.h"
#include "tdfui/view/label.h"

namespace tdfcore {
/**
 * 按钮的状态，普通状态、选取状态、普通高亮状态、选取高亮状态、禁用状态
 * 后续可以参考ios实现，抽象出UIControl继承于View
 */
enum class ButtonState {
  kNormal         = 0,        // 表示控件处于正常状态
  kHighlighted    = 1 << 0,   // 表示控件处于高亮状态，焦点选中、按下时
  kDisable        = 1 << 1,   // 表示控件出于被禁用状态，无法响应用户交互
  kHover          = 1 << 2,   // 表示控件处于鼠标悬停状态
  kSelected       = 1 << 3,   // 表示控件处于选中状态，按下后未抬起状态
};

/**
 * @brief 按钮控件
 * @details 继承自 component ViewGroup 组合了ImageView和TextView
 */
class Button : public View {
  TDF_REFF_CLASS_META(Button)

 public:
  void Init() override;

  /**
   * 自适应大小
   */
  void SizeToFit();

  /**
   * 设置button的状态对应的背景颜色
   */
  void SetBackgroundColor(Color color, ButtonState state);

  /**
   * 获取button的状态对应的背景颜色
   */
  Color GetBackgroundColor(ButtonState state) const;

  /**
   * 设置button的状态对应的边框颜色
   */
  void SetBorderColor(Color color, ButtonState state);

  /**
   * 获取button的状态对应的边框颜色
   */
  Color GetBorderColor(ButtonState state) const;

  /**
   * 设置按钮上状态对应的文字内容
   */
  void SetTitle(const std::u16string &text, ButtonState state);

  /**
   * 获取按钮上状态对应的文字内容
   */
  const std::u16string &GetTitle(ButtonState state) const;

  /**
   * 设置按钮上状态对应的文字颜色
   */
  void SetTitleColor(Color color, ButtonState state);

  /**
   * 获取按钮上状态对应的文字颜色
   */
  Color GetTitleColor(ButtonState state) const;

  /**
   * 设置字体大小
   */
  void SetFontSize(float size, ButtonState state);

  /**
   * 获取字体大小
   */
  float GetFontSize(ButtonState state) const;

  /**
   * 设置字体风格
   */
  void SetTextStyle(TextStyle text_style, ButtonState state);
  /**
   * 获取字体风格
   */
  TextStyle GetTextStyle(ButtonState state) const;

  /**
   * 设置按钮上状态对应的图像
   */
  void SetImage(const std::shared_ptr<Image> &image, ButtonState state);

  /**
   * 获取按钮上状态对应的图像
   */
  const std::shared_ptr<Image> &GetImage(ButtonState state) const;

  /**
   * 设置图像的大小，和image的frame属性不同的是frame有可能发生改变
   * @param size
   */
  void SetImageSize(const TSize &size) { image_size_ = size; }

  constexpr const TSize &GetImageSize() const { return image_size_; }

  /**
   * 设置按钮上状态对应的背景图像
   */
  void SetBackgroundImage(const std::shared_ptr<Image> &image, ButtonState state);

  /**
   * 获取按钮上状态对应的背景图像
   */
  const std::shared_ptr<Image> &GetBackgroundImage(ButtonState state) const;

  /**
   * 设置 文字 在 文字和图片 这个整体中的位置， 图片 默认在左半区域
   *  |————————————————————————————————|
   *  |image | title_edge_insets  title|
   *  |————————————————————————————————|
   *  文字的edge_insets left 是对于image的
   */
  void SetTitleEdgeInsets(const EdgeInsets &edge_insets);

  /**
   * 获取 文字 在 文字和图片 这个整体中的位置， 文字 默认在右半区域
   */
  constexpr const EdgeInsets &GetTitleEdgeInsets() const { return title_edge_insets_; }

  /**
   * 设置 图片 在 文字和图片 这个整体中的位置，图片 默认在左半区域
   */
  void SetImageEdgeInsets(const EdgeInsets &edge_insets);

  /**
   * 获取 图片 在 文字和图片 这个整体中的位置，文字 默认在右半区域
   */
  constexpr const EdgeInsets &GetImageEdgeInsets() const { return image_edge_insets_; }

  /**
   * 设置 文字和图片 这一个整体在按钮中的位置 默认是四周的边距为0
   */
  void SetContentEdgeInsets(const EdgeInsets &edge_insets);

  /**
   * 获取 文字和图片 这一个整体在按钮中的位置 默认是四周的边距为0
   */
  constexpr const EdgeInsets &GetContentEdgeInsets() const { return content_edge_insets_; }

  /**
   * 获取 文字 对应的view
   */
  constexpr const std::shared_ptr<Label> &GetLabelView() const { return label_; }

  /**
   * 获取 图像 对应的view
   */
  constexpr const std::shared_ptr<ImageView> &GetImageView() const { return image_; }

  /**
   * 获取 背景图像 对应的view
   */
  constexpr const std::shared_ptr<ImageView> &GetBackgroundImageView() const { return background_image_; }

  /**
   * 设置按钮的点击回调
   */
  void SetTapListener(const TapClosure &closure);

  /**
   * 获取按钮的点击回调
   */
  constexpr const TapClosure &GetTapListener() const { return closure_; }

  /**
   * 设置当前状态
   */
  void SetState(ButtonState state);

  /**
   * 获取当前状态
   */
  constexpr ButtonState GetState() { return current_state_; }

  /**
   * 获取当前是否为选中状态
   */
  constexpr bool IsSelected() const { return is_selected_; }

  /**
   * @brief 设置鼠标选中悬浮颜色
   * @param color 选中悬浮颜色
   */
  void SetHoverColor(Color color) { hover_color_ = color; }

  /**
   * 设置当前是否为选中状态
   */
  void SetSelection(bool is_selected);

  void Layout() override;

 protected:
  explicit Button(const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent()) : View(view_context) {}

  /**
   * 用于构造 按钮类型 对应的配置信息，如MainType对应的背景颜色等信息
   */
  struct ButtonStyle {
    std::optional<std::u16string> text_;
    std::optional<float> font_size_;
    std::optional<Color> text_color_;
    std::optional<TextStyle> text_style_;
    std::optional<Color> background_color_;
    std::optional<Color> border_color_;
    std::shared_ptr<Image> image_;
    std::shared_ptr<Image> background_image_;
  };

 private:
  void SetDefaultConfiguration();
  void UpdateButtonAttributes();
  void UpdateButtonColors(ButtonState state);
  void UpdateButtonLabels(ButtonState state);
  void UpdateButtonImages(ButtonState state);
  void CalculateImageHorizontalLayout() const;
  void CalculateImageVerticalLayout() const;
  void CalculateTitleHorizontalLayout() const;
  void CalculateTitleVerticalLayout() const;
  void CalculateImageLayout() const;
  void CalculateTitleLayout() const;
  void ResetHoverState();

  std::shared_ptr<Label> label_;
  TSize image_size_;
  std::shared_ptr<ImageView> image_;
  std::shared_ptr<View> content_;
  std::shared_ptr<ImageView> background_image_;
  EdgeInsets title_edge_insets_;
  EdgeInsets image_edge_insets_;
  EdgeInsets content_edge_insets_;
  std::shared_ptr<TapGestureRecognizer> tap_gesture_;
  std::shared_ptr<HoverGestureRecognizer> hover_gesture_;
  std::unordered_map<ButtonState, ButtonStyle> states_;
  TapClosure closure_;
  ButtonState current_state_ = ButtonState::kNormal;
  bool is_selected_;

  // Hover相关属性
  Color hover_color_ = Color::Transparent();
  bool is_hovered_ = false;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::Button, bases<tdfcore::View>)
TDF_REFL_END(Button)
