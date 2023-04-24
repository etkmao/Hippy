//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/common/value_change_notifier.h"
#include "core/support/animation/animation_controller.h"
#include "core/support/gesture/recognizer/drag_detail.h"
#include "tdfui/view/image_view.h"

namespace tdfcore {
class Switch : public View, public ValueNotifier<bool> {
  TDF_REFF_CLASS_META(Switch)

 public:
  void Init() override;

  /**
   * @brief 设置是否开启
   * @param is_on  是否开启
   * @param animated  是否带动画
   */
  void SetOn(bool is_on, bool animated = false);

  /**
   * @brief 获取是否开启
   */
  bool IsOn() const { return GetValue(); }

  /**
   * @brief 设置开启时的背景颜色
   * @param color  颜色
   */
  void SetOnColor(Color color);

  /**
   * @brief 获取开启时的背景颜色
   * @param color  颜色
   */
  Color GetOnColor() const { return on_color_; }

  /**
   * @brief 设置关闭时的背景颜色
   * @param color  颜色
   */
  void SetOffColor(Color color);

  /**
   * @brief 获取关闭时的背景颜色
   */
  Color GetOffColor() const { return off_color_; }

  /**
   * @brief 设置操作按钮颜色
   * @param color  颜色
   */
  void SetThumbColor(Color color);

  /**
   * @brief 获取操作按钮颜色
   */
  Color GetThumbColor() const { return thumb_color_; }

  /**
   * @brief 设置是否圆形圆角
   * @param is_rounded
   */
  void SetIsRounded(bool is_rounded);

  /**
   * @brief 获取是否圆形圆角
   */
  bool IsRounded() const { return is_rounded_; }

  /**
   * @brief 设置开启时的背景图片
   * @param image  图片
   */
  void SetOnImage(const std::shared_ptr<Image>& image);

  /**
   * @brief 获取开启时的背景图片
   */
  std::shared_ptr<Image> GetOnImage() const;

  /**
   * @brief 设置关闭时的背景图片
   * @param image  图片
   */
  void SetOffImage(const std::shared_ptr<Image>& image);

  /**
   * @brief 获取关闭时的背景图片
   */
  std::shared_ptr<Image> GetOffImage() const;

 protected:
  explicit Switch(bool is_on = false, const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent());
  void Load() override;
  void Layout() override;

 private:
  void AddGestureEvent();
  void SetUpAnimations();
  void ShowOn(bool animated);
  void ShowOff(bool animated);
  void UpdateThumbExtent(double percent);
  void UpdateThumbPosition(double percent);
  void UpdateBackgroundColor(Color color);
  void HandleTapDown();
  void HandleTapUp();
  void HandleTap();
  void HandleTapCancel();
  void HandleDragStart();
  void HandleDragUpdate(const DragUpdateDetails &details);
  void HandleDragEnd();

  bool is_rounded_ = true;
  Color on_color_;
  Color off_color_;
  Color thumb_color_;
  std::shared_ptr<View> background_view_;
  std::shared_ptr<View> thumb_view_;
  std::shared_ptr<ImageView> on_image_view_;
  std::shared_ptr<ImageView> off_image_view_;
  std::shared_ptr<Image> on_image_;
  std::shared_ptr<Image> off_image_;
  bool is_animating_ = false;
  bool current_changed_value_ = false;
  double drag_delta_ = 0.0;
  std::shared_ptr<Animation<Color>> color_animation_;
  std::shared_ptr<AnimationController> extent_animation_controller_;
  std::shared_ptr<AnimationController> position_animation_controller_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::Switch, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::Switch)
