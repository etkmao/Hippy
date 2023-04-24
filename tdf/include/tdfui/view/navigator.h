//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/recognizer/horizontal_pan_gesture_recognizer.h"
#include "tdfui/view/navigator_transition.h"

namespace tdfcore {
/**
 * @brief 导航组件
 * @details 维护了一个页面栈，支持自定义转场动画
 */
class Navigator : public View {
  TDF_REFF_CLASS_META(Navigator)

 public:
  /**
   * @brief 获取页面对应的Navigator
   * @param view
   */
  static std::shared_ptr<Navigator> Of(const std::shared_ptr<View>& view);

  /**
   * @brief push一个新的页面
   * @param view 新页面
   * @param animated 是否带动画
   * @param transition 自定义转场动画，传空或不传则使用默认动画
   */
  void Push(const std::shared_ptr<View>& view, bool animated,
            const std::shared_ptr<NavigatorTransition>& transition = nullptr);

  /**
   * @brief pop当前最上层页面
   * @param animated 是否带动画
   * @param transition 自定义转场动画，传空或不传则使用默认动画
   */
  void Pop(bool animated, const std::shared_ptr<NavigatorTransition>& transition = nullptr);

  /**
   * @brief pop到某个页面
   * @param view 页面
   * @param animated 是否带动画
   * @param transition 自定义转场动画，传空或不传则使用默认动画
   */
  void PopTo(const std::shared_ptr<View>& view, bool animated,
             const std::shared_ptr<NavigatorTransition>& transition = nullptr);

  /**
   * @brief pop到根页面
   * @param animated 是否带动画
   * @param transition 自定义转场动画，传空或不传则使用默认动画
   */
  void PopToRoot(bool animated, const std::shared_ptr<NavigatorTransition>& transition = nullptr);

  /**
   * @brief 设置navigator push转场动画，所有push操作都使用设置后的动画
   * @param transition
   */
  void SetPushTransition(const std::shared_ptr<NavigatorTransition>& transition) { push_transition_ = transition; }

  /**
   * @brief 设置pop转场动画，所有pop操作都使用设置后的动画
   * @param transition
   */
  void SetPopTransition(const std::shared_ptr<NavigatorTransition>& transition) { pop_transition_ = transition; }

  /**
   * @brief 是否可以pop
   */
  bool CanPop() const;

  /**
   * @brief 设置是否开启边缘返回手势
   */
  void SetIsEnableEdgeBack(bool enable) { is_edge_back_enable_ = enable; }

  /**
   * @brief 获取是否已开启边缘返回手势
   */
  bool GetIsEnableEdgeBack() const { return is_edge_back_enable_; }

  /**
   * @brief 获取当前页面栈
   */
  constexpr const std::vector<std::shared_ptr<View>>& GetHistory() { return history_; }

 protected:
  explicit Navigator(std::shared_ptr<View> root_view,
                     const std::shared_ptr<ViewContext>& view_context = ViewContext::GetCurrent());
  void Load() override;
  void Layout() override;
  void SetRootView(const std::shared_ptr<View>& root_view);

 private:
  TRect GetVisibleBounds() const;
  void PushOrInvokeOperation(const std::function<void()>& closure);
  void PerformNextOperation();
  void LayoutVisibleView();
  void UpdateVisibleView(bool animated, const std::shared_ptr<NavigatorTransition>& transition = nullptr);
  void AddEdgeGestureViewIfNeed();
  void EnableEdgeGesture(bool enable);
  void HandleDragStart(const DragStartDetails& details);
  void HandleDragUpdate(const DragUpdateDetails& details);
  void HandleDragEnd(const DragEndDetails& details);
  void HandleDragCancel(const DragCancelDetails& details);

  std::shared_ptr<NavigatorTransition> push_transition_;
  std::shared_ptr<NavigatorTransition> pop_transition_;
  std::shared_ptr<NavigatorTransition> animating_transition_;
  bool is_animating_ = false;
  bool is_dragging_ = false;

  std::queue<std::function<void()>> operations_;
  std::vector<std::shared_ptr<View>> history_;
  std::shared_ptr<View> root_view_;
  std::shared_ptr<View> wrapper_view_;
  std::shared_ptr<View> visible_view_;

  std::shared_ptr<HorizontalPanGestureRecognizer> gesture_;
  std::shared_ptr<View> edge_gesture_view_;
  bool is_edge_back_enable_ = true;
  double edge_back_gesture_progress_ = 0;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::Navigator, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::Navigator)
