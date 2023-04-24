//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/value_change_notifier.h"
#include "core/support/animation/animation_controller.h"
#include "core/support/gesture/recognizer/horizontal_pan_gesture_recognizer.h"
#include "core/support/gesture/recognizer/tap_gesture_recognizer.h"
#include "tdfui/view/image_view.h"

namespace tdfcore {
class Slider : public View, public ValueNotifier<double> {
  TDF_REFF_CLASS_META(Slider)

 public:
  /**
   * @brief 初始化
   */
  void Init() override;

  /**
   * @brief 设置最小值
   * @param value 最小值
   */
  void SetMinimumValue(double value);

  /**
   * @brief 获取最小值
   * @return 最小值
   */
  constexpr double GetMinimumValue() const { return minimum_value_; }

  /**
   * @brief 设置最大值
   * @param value 最大值
   */
  void SetMaximumValue(double value);

  /**
   * @brief 获取最大值
   * @return 最大值
   */
  constexpr double GetMaximumValue() const { return maximum_value_; }

  /**
   * @brief 设置最小值颜色（滑块左边部分）
   * @param color 颜色
   */
  void SetMinimumTintColor(Color color);

  /**
   * @brief 获取最小值颜色（滑块左边部分）
   * @return 颜色
   */
  constexpr Color GetMinimumTintColor() const { return minimum_tint_color_; }

  /**
   * @brief 设置最大值颜色（滑块右边部分）
   * @param color 颜色
   */
  void SetMaximumTintColor(Color color);

  /**
   * @brief 获取最大值颜色（滑块右边部分）
   * @return 颜色
   */
  constexpr const Color &GetMaximumTintColor() const { return maximum_tint_color_; }

  /**
   * @brief 设置滑块颜色
   * @param color 颜色
   */
  void SetThumbTintColor(Color color);

  /**
   * @brief 设置滑块border颜色
   * @param color 颜色
   */
  void SetThumbBorderColor(Color color);

  /**
   * @brief 获取滑块颜色
   * @return 颜色
   */
  constexpr const Color &GetThumbTintColor() const { return thumb_tint_color_; }

  /**
   * @brief 设置滑块图片
   * @param image 图片
   */
  void SetThumbImage(const std::shared_ptr<Image> &image);

  /**
   * @brief 获取滑块图片
   * @return 图片
   */
  std::shared_ptr<Image> GetThumbImage() const;

  /**
   * @brief 设置滑块大小
   * @param size 大小
   */
  void SetThumbSize(const TSize &size);

  /**
   * @brief 获取滑块大小
   * @return 大小
   */
  constexpr const TSize &GetThumbSize() const { return thumb_size_; }

  /**
   * @brief 设置最小值辅助视图
   * 该视图将显示在滑动条左边的位置
   * @param view 视图
   */
  void SetMinimumAccessoryView(const std::shared_ptr<View> &view);

  /**
   * @brief 获取最小值辅助视图
   * 显示在滑动条左边的位置的视图
   * @return 视图
   */
  constexpr const std::shared_ptr<View> &GetMinimumAccessoryView() const { return minimum_accessory_view_; }

  /**
   * @brief 设置最大值辅助视图
   * 该视图将显示在滑动条右边的位置
   * @param view 视图
   */
  void SetMaximumAccessoryView(const std::shared_ptr<View> &view);

  /**
   * @brief 获取最大值辅助视图
   * 显示在滑动条右边的位置的视图
   * @return 视图
   */
  constexpr const std::shared_ptr<View> &GetMaximumAccessoryView() const { return maximum_accessory_view_; }

  /**
   * @brief 获取当前进度
   * @return 返回百分比
   */
  constexpr double GetProgress() const {
    assert(maximum_value_ != 0);
    return (GetValue() - minimum_value_) / maximum_value_;
  }

  /**
   * @brief 设置当前值
   * @param value 值
   * @param is_animated 是否使用动画
   */
  void SetValue(double value, bool is_animated = false);

  /**
   * @brief 设置drag手势结束监听
   */
  void AddDragEndListener(const DragEndClosure &listener) const;

  /**
   * @brief 设置drag手势开始监听
   */
  void AddDragStartListener(const DragStartClosure &listener) const;

  /**
   * @brief 设置drag手势down监听
   */
  void AddDragDownListener(const DragDownClosure &listener) const;

  /**
   * @brief 设置drag手势cancel监听
   */
  void AddDragCancelListener(const DragCancleClosure &listener) const;

 protected:
  explicit Slider(const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent());
  void Layout() override;

  void SetThumbClearStyle();
  void SetThumbDefaultStyle();

  double minimum_value_ = 0;
  double maximum_value_ = 1;
  Color minimum_tint_color_;
  Color maximum_tint_color_;
  Color thumb_tint_color_;
  Color thumb_border_color_;
  TSize thumb_size_;
  std::shared_ptr<View> minimum_view_;
  std::shared_ptr<View> maximum_view_;
  std::shared_ptr<ImageView> thumb_view_;
  std::shared_ptr<View> minimum_accessory_view_;
  std::shared_ptr<View> maximum_accessory_view_;
  std::shared_ptr<AnimationController> animation_controller_;
  std::shared_ptr<ScrollGestureRecognizer> scroll_gesture_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::Slider, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::Slider)
