//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <utility>

#include "core/common/object.h"
#include "core/support/animation/animation.h"
#include "core/support/animation/animation_controller.h"
#include "tdfui/view/transition_context.h"

namespace tdfcore {
/**
 * @brief 转场动画，转场完成后须调用context的completer
 */
class NavigatorTransition : public Object {
 public:
  /**
   * @brief 开启过场动画
   * @param context
   */
  virtual void StartAnimateTransition(std::unique_ptr<TransitionContext> context) = 0;

  /**
   * @brief 进入交互模式
   */
  virtual void EnterInteractiveMode() { is_interactive_mode_ = true; }

  /**
   * @brief 离开交互模式
   */
  virtual void LeaveInteractiveMode() { is_interactive_mode_ = false; }

  /**
   * @brief 更新交互的进度，一般在外部手势回调中更新进度
   * @param progress 0.0~1.0
   */
  virtual void UpdateInteractiveProgress(double progress) {}

  /**
   * @brief 交互转场取消，需要恢复container view现场
   */
  virtual void CancelInteractive() {}

  /**
   * @brief 交互转场完成
   */
  virtual void FinishInteractive() {}

 protected:
  bool is_interactive_mode_ = false;
  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 默认滑动转场效果
 * @details 支持向左滑入向右滑出以及向上滑入向下滑出
 */
class SlideNavigatorTransition : public NavigatorTransition,
                                 public std::enable_shared_from_this<SlideNavigatorTransition> {
 public:
  ~SlideNavigatorTransition() override = default;
  enum class SlideDirection { kHorizontal, kVertical };
  void StartAnimateTransition(std::unique_ptr<TransitionContext> context) override;
  void UpdateInteractiveProgress(double progress) override;
  void CancelInteractive() override;
  void FinishInteractive() override;

 protected:
  explicit SlideNavigatorTransition(SlideDirection direction = SlideDirection::kHorizontal) : direction_(direction) {}

 private:
  SlideDirection direction_;
  std::shared_ptr<AnimationController> animation_controller_;
  std::unique_ptr<TransitionContext> context_;
  std::shared_ptr<Animation<TM33>> animation_;
  TM33 transform_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
