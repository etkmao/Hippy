//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/value_change_notifier.h"
#include "core/support/animation/animation_controller.h"
#include "core/support/gesture/recognizer/horizontal_pan_gesture_recognizer.h"
#include "core/support/gesture/recognizer/pan_gesture_recognizer.h"
#include "core/support/gesture/recognizer/tap_gesture_recognizer.h"
#include "core/support/gesture/recognizer/vertical_pan_gesture_recognizer.h"
#include "tdfui/view/mouse_listener.h"
#include "tdfui/view/view.h"

namespace tdfcore {
enum class ScrollBarFadeStatus {
  kUnfaded,
  kFading,
  kFaded,
};
enum class ScrollBarMode {
  kVertical,
  kHorizontal,
};
/**
 * 自定义scrollbar组件
 */
class ScrollBar : public View, public ValueNotifier<double> {
  TDF_REFF_CLASS_META(ScrollBar)

 public:
  ScrollBar();
  ~ScrollBar() override = default;
  void Init() override;
  /**
   * @brief 设置Scrollbar是否要一直展示 默认为true
   * @param is_always_shown
   */
  void SetIsAlwaysShown(bool is_always_shown);

  /**
   * @brief 设置滑块占比
   * @param thumb_length_scale
   */
  void SetThumbLengthScale(double thumb_length_scale);

  /**
   * @brief 设置轨道颜色
   * @param track_color
   */
  void SetTrackColor(Color track_color);

  /**
   * @brief 设置滑动条颜色
   * @param thumb_color
   */
  void SetThumbColor(Color thumb_color);

  /**
   * @brief 设置滑动条边框颜色
   * @param thumb_border_color
   */
  void SetThumbBorderColor(Color thumb_border_color);

  /**
   * @brief 设置轨道边框颜色
   * @param track_border_color
   */
  void SetTrackBorderColor(Color track_border_color);

  /**
   * @brief 设置组件在无操作条件下从展示到消失之间的时间间隔
   * @param fading_delay
   */
  void SetFadingDelay(TDuration fading_delay);

  /**
   * @brief 得到现在的进度百分比
   * @return
   */
  double GetProgress() const;

  /**
   * @brief 设置滑块
   */
  void SetThumbView(const std::shared_ptr<View>& thumb_view);

  /**
   * @brief 设置当前进度来更新位置
   * @param progress 当前进度
   */
  void SetProgress(double progress);

  /**
   * @brief 设置scrollbar的模式, 默认为vertical
   */
  void SetScrollBarMode(ScrollBarMode mode);

  /**
   * 设置组件淡出动画时长
   * @param fade_in_duration
   */
  void SetFadeInDuration(TDuration fade_in_duration);

  /**
   * 设置组件淡入动画时长
   * @param fade_out_duration
   */
  void SetFadeOutDuration(TDuration fade_out_duration);

 private:
  std::shared_ptr<Timer> fading_timer_;
  bool is_always_shown_ = true;
  bool is_dragging_ = false;
  bool is_tapping_ = false;
  TDuration fading_delay_;
  TDuration fade_out_duration_;
  TDuration fade_in_duration_;
  double track_radius_ = 0;
  double thumb_radius_ = 0;
  double thumb_height_ = 0;
  double thumb_length_scale_ = 0.1;
  double track_length_ = 0;
  double begin_track_length_ = 0;
  double end_track_length_ = 0;
  double minimum_value_ = 0;
  double maximum_value_ = 1;
  Color track_color_;
  Color track_border_color_;
  Color thumb_color_;
  Color thumb_border_color_;
  std::shared_ptr<ScrollGestureRecognizer> horizontal_scroll_gesture_;
  std::shared_ptr<VerticalPanGestureRecognizer> vertical_scroll_gesture_;
  std::shared_ptr<TapGestureRecognizer> tap_gesture_;
  std::shared_ptr<View> begin_track_view_;
  std::shared_ptr<View> end_track_view_;
  std::shared_ptr<View> thumb_view_;
  ScrollBarMode scroll_bar_mode_ = ScrollBarMode::kVertical;
  std::shared_ptr<tdfcore::MouseListener> thumb_mouse_listener_;
  std::shared_ptr<tdfcore::MouseListener> begin_track_mouse_listener_;
  std::shared_ptr<tdfcore::MouseListener> end_track_mouse_listener_;
  ScrollBarFadeStatus scroll_bar_fade_status_ = ScrollBarFadeStatus::kUnfaded;
  void Layout() override;
  void Unmount() override;
  void HandleVerticalScrollGesture();
  void HandleHorizontalScrollGesture();
  void HandleTapGesture(ScrollBarMode mode);
  void SetDragEndListener(const DragEndClosure& listener);
  void SetDragStartListener(const DragStartClosure& listener);
  void SetDragDownListener(const DragDownClosure& listener);
  void SetDragCancelListener(const DragCancleClosure& listener);
  void SetTapDownListener(const TapDownClosure& listener);
  void FadeOut(TDuration duration);
  void FadeIn();
  void ScrollBarDefaultStyle();
  void HandleMouseEvent();
  void InternalSetProgress(double progress);
  void HandleOverScrollEvent(double progress);
  double InternalGetProgress() const;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ScrollBar, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::ScrollBar)
