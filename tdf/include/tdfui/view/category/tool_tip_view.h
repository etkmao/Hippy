//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <iostream>
#include "core/support/gesture/recognizer/hover_gesture_recognizer.h"
#include "tdfui/view/view.h"

namespace tdfcore {
class Label;

class ToolTipView : public View {
  TDF_REFF_CLASS_META(ToolTipView)

 public:
  void Init() override;
  ~ToolTipView();

  /**
   * 创建一个ToolTipView，返回的实例可以自由定制
   */
  static std::shared_ptr<ToolTipView> AttachToView(const std::shared_ptr<View> &view);

  /**
   * 设置hover状态出现的tips
   */
  void SetText(const std::u16string &text);

  /**
   * 设置hover状态出现的view
   */
  void SetContentView(const std::shared_ptr<View> &view);

  /**
   *  设置Tooltip延迟出现的时间
   */
  constexpr void SetTooltipWaitDuration(TDuration duration) { tooltip_wait_duration_ = duration; }

 private:
  explicit ToolTipView(const std::shared_ptr<View> &view);
  void ShowTooltip(const TPoint position = TPoint::Make(0, 0));
  void SetupStyleAndAnimation();
  void removeToolTip();

  std::shared_ptr<HoverGestureRecognizer> hover_gesture_;
  std::weak_ptr<View> current_attach_view_;
  std::shared_ptr<AnimationController> animation_controller_;
  std::function<void(const double percent)> animation_finish_;
  TDuration tooltip_wait_duration_;
  bool entered_ = false;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ToolTipView, bases < tdfcore::View >)
TDF_REFL_END(tdfcore::ToolTipView)