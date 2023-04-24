//
// Copyright (c) 2020 Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include "core/common/listener.h"
#include "core/engine/render/transform_object.h"
#include "core/support/animation/animation_controller.h"
#include "core/support/animation/ticker.h"
#include "core/support/gesture/group/gesture_recognizer_group.h"
#include "core/support/gesture/recognizer/horizontal_pan_gesture_recognizer.h"
#include "core/support/gesture/recognizer/pan_gesture_recognizer.h"
#include "core/support/gesture/recognizer/pinch_rotate_gesture_recognizer.h"
#include "core/support/gesture/recognizer/vertical_pan_gesture_recognizer.h"
#include "core/support/gesture/recognizer/tap_gesture_recognizer.h"
#include "core/support/physics/scroll_physics.h"
#include "tdfui/view/scroll_view_header.h"
#include "tdfui/view/view.h"

namespace tdfcore {
class ScrollView;

enum class ScrollDirection {
  kBoth, kHorizontal, kVertical
};

class ScrollView : public View {
  TDF_REFF_CLASS_META(ScrollView)

 public:
  ~ScrollView() override = default;

  /**
   * @brief 设置scroll view的内容大小
   * @param content_rect 内容大小
   */
  virtual void SetContentRect(TRect content_rect);

  /**
   * @brief 获取内容大小
   * @return 大小
   */
  TRect GetContentRect() const { return content_rect_; }

  /**
   * @brief 设置滑动方向
   * 设置单一方向后，其余方向将无法滑动（包括越界滑动）
   */
  void SetScrollDirection(ScrollDirection scroll_direction);

  /**
   * @brief 获取滑动方向
   */
  constexpr ScrollDirection GetScrollDirection() const { return scroll_direction_; }

  /**
   * @brief 设置是否允许横向 越界滑动
   * @param scroll_enabled 是否可以滚动
   */
  void SetHorizontalOverscrollEnabled(bool scroll_enabled);

  /**
   * @brief 获取是否允许横向 越界滑动
   * @return 是否可以滚动
   */
  bool GetHorizontalOverscrollEnabled() const { return scroll_physics_->GetHorizontalOverScrollEnabled(); }

  /**
   * @brief 设置是否允许纵向 越界滑动
   * @param scroll_enabled  是否可以滚动
   */
  void SetVerticalOverscrollEnabled(bool scroll_enabled);

  /**
   * @brief 获取是否允许纵向 越界滑动
   * @return  是否可以滚动
   */
  bool GetVerticalOverscrollEnabled() const { return scroll_physics_->GetVerticalOverScrollEnabled(); }

  /**
   * @brief 获取目前是否为over scroll 状态
   * @return over scorll 状态
   */
  bool IsOverScrolled() const { return scroll_physics_->IsOverScrolled(); }

  /**
   * @brief 滚动到目标位置, 滚动过程中是否需要动画
   * @param offset 偏移 animated 动画
   */
  void SetOffset(const TPoint &offset, bool animated = false);

  /**
   * @brief 获取子view相对于ScrollView的偏移
   * @return 位移
   */
  constexpr TPoint GetOffset() const { return offset_; }

  /**
   * @brief 设置当前的速度
   * @param velocity 速度
   */
  void SetVelocity(TPoint velocity) { current_velocity_ = velocity; }

  /**
   * @brief 获取当前的速度
   * @return 速度
   */
  constexpr TPoint GetVelocity() const { return current_velocity_; }

  /**
   * @brief 设置是否可以滑动
   * @param is_enabled 是否可以滑动
   */
  void SetScrollEnabled(bool is_enabled);

  /**
   * @brief 获取是否可以滑动
   * @return enableScroll 是否可以滑动
   */
  constexpr bool GetScrollEnabled() const { return scroll_gesture_recognizer_->IsEnabled(); }

  /**
   * @brief 设置是否可以缩放
   * @param is_enabled 是否可以缩放
   */
  void SetScaleEnabled(bool is_enabled);

  /**
   * @brief 获取是否可以缩放
   * @return  是否可以缩放
   */
  constexpr bool GetScaleEnabled() const { return scale_gesture_recognizer_->IsEnabled(); }

  /**
   * @brief 设置最大比例
   * @param max_size 比例
   */
  void SetMaxScaleSize(TScalar max_size) { max_scale_size_ = max_size; }

  /**
   * @brief 获取最大比例
   * @return 比例
   */
  constexpr TScalar GetMaxScaleSize() const { return max_scale_size_; }

  /**
   * @brief 设置最小比例
   * @param min_size 比例
   */
  void SetMinScaleSize(TScalar min_size) { min_scale_size_ = min_size; }

  /**
   * @brief 获取最小比例
   * @return 比例
   */
  constexpr TScalar GetMinScaleSize() const { return min_scale_size_; }

  /**
   * @brief 设置物理效果
   * @param scroll_physics 物理效果
   */
  void SetScrollPhysics(const std::shared_ptr<ScrollPhysics> &scroll_physics) { scroll_physics_ = scroll_physics; }

  /**
   * @brief 获取物理效果
   * @return 物理效果
   */
  constexpr const std::shared_ptr<ScrollPhysics> &GetScrollPhysics() const { return scroll_physics_; }

  /**
   * @brief 获取当前缩放比例
   * @return 返回缩放比例
   */
  constexpr TScalar GetScale() const { return current_scale_; }

  /**
   * @brief 以ScrollView中的坐标为锚点，设置缩放比例。
   * @param scale 缩放比例
   */
  void SetScale(TScalar scale, TPoint point = {0, 0});

  TM33 GetChildTransform() const { return child_transform_->GetTransform(); }

  /**
   * @brief 注册一个 任何原因导致滑动开始 的监听者
   * @return 监听者对应的id
   */
  uint64_t AddScrollStartListener(const std::function<void()> &listener) {
    return scroll_start_listener_.Add(listener);
  }

  /**
   * @brief 注销一个 任何原因导致滑动开始 的监听者
   * @param id 监听者对应的id
   */
  void RemoveScrollStartListener(uint64_t id) { scroll_start_listener_.Remove(id); }

  /**
   * @brief 注册一个 任何原因导致滑动的位移更新 的监听者
   * @return 监听者对应的id
   */
  uint64_t AddScrollUpdateListener(const std::function<void(TPoint, TPoint)> &listener) {
    return scroll_update_listener_.Add(listener);
  }

  /**
   * @brief 注销一个 任何原因导致滑动的位移更新 的监听者
   * @param id 监听者对应的id
   */
  void RemoveScrollUpdateListener(uint64_t id) { scroll_update_listener_.Remove(id); }

  /**
   * @brief 注册一个 任何原因导致滑动结束 的监听者
   * @return 监听者对应的id
   */
  uint64_t AddScrollEndListener(const std::function<void()> &listener) { return scroll_end_listener_.Add(listener); }

  /**
   * @brief 注销一个 任何原因导致滑动结束 的监听者
   * @param id 监听者对应的id
   */
  void RemoveScrollEndListener(uint64_t id) { scroll_end_listener_.Remove(id); }

  /**
   * @brief 注册一个 拖动开始 的监听者
   * @return 监听者对应的id
   */
  uint64_t AddDragStartListener(const std::function<void()> &listener) { return drag_start_listener_.Add(listener); }

  /**
   * @brief 注销一个 拖动开始 的监听者
   * @param id 监听者对应的id
   */
  void RemoveDragStartListener(uint64_t id) { drag_start_listener_.Remove(id); }

  /**
   * @brief 注册一个 拖动导致的位移更新 的监听者
   * @return 监听者对应的id
   */
  uint64_t AddDragUpdateListener(const std::function<void(TPoint, TPoint)> &listener) {
    return drag_update_listener_.Add(listener);
  }

  /**
   * @brief 注销一个 拖动导致的位移更新 的监听者
   * @param id 监听者对应的id
   */
  void RemoveDragUpdateListener(uint64_t id) { drag_update_listener_.Remove(id); }

  /**
   * @brief 注册一个 拖动结束 的监听者
   * @return 返回该listener对应的id
   */
  uint64_t AddDragEndListener(const std::function<void()> &listener) { return drag_end_listener_.Add(listener); }

  /**
   * @brief 注销一个 拖动结束 的监听者
   * @param id 监听者对应的id
   */
  void RemoveDragEndListener(uint64_t id) { drag_end_listener_.Remove(id); }

  /**
   * @brief 注册一个 物理效果开始 的监听者
   * @return 返回该listener对应的id
   */
  uint64_t AddFlingStartListener(const std::function<void()> &listener) { return fling_start_listener_.Add(listener); }

  /**
   * @brief 注销一个 物理效果开始 的监听者
   * @param id 监听者对应的id
   */
  void RemoveFlingStartListener(uint64_t id) { fling_start_listener_.Remove(id); }

  /**
   * @brief 注册一个 物理效果导致的位移更新 的监听者
   * @return 返回该listener对应的id
   */
  uint64_t AddFlingUpdateListener(const std::function<void(TPoint, TPoint)> &listener) {
    return fling_update_listener_.Add(listener);
  }

  /**
   * @brief 注销一个 物理效果导致的位移更新 的监听者
   * @param id 监听者对应的id
   */
  void RemoveFlingUpdateListener(uint64_t id) { fling_update_listener_.Remove(id); }

  /**
   * @brief 注册一个 物理效果结束 的监听者
   * @return 返回该listener对应的id
   */
  uint64_t AddFlingEndListener(const std::function<void()> &listener) { return fling_end_listener_.Add(listener); }

  /**
   * @brief 注销一个 物理效果结束 的监听者
   * @param id 监听者对应的id
   */
  void RemoveFlingEndListener(uint64_t id) { fling_end_listener_.Remove(id); }

  /**
   * @brief 注册一个 缩放开始 的监听者
   * @return 返回该listener对应的id
   */
  uint64_t AddScaleStartListener(const std::function<void()> &listener) { return scale_start_listener_.Add(listener); }

  /**
   * @brief 注销一个 缩放开始 的监听者
   * @param id 监听者对应的id
   */
  void RemoveScaleStartListener(uint64_t id) { scale_start_listener_.Remove(id); }

  /**
   * @brief 注册一个 缩放更新 的监听者
   * @return 返回该listener对应的id
   */
  uint64_t AddScaleUpdateListener(const std::function<void(double)> &listener) {
    return scale_update_listener_.Add(listener);
  }

  /**
   * @brief 注销一个 缩放更新 的监听者
   * @param id 监听者对应的id
   */
  void RemoveScaleUpdateListener(uint64_t id) { scale_update_listener_.Remove(id); }

  /**
   * @brief 注册一个 缩放结束 的监听者
   * @return 返回该listener对应的id
   */
  uint64_t AddScaleEndListener(const std::function<void()> &listener) { return scale_end_listener_.Add(listener); }

  /**
   * @brief 注销一个 缩放结束 的监听者
   */
  void RemoveScaleEndListener(uint64_t id) { scale_end_listener_.Remove(id); }

  /**
   * @brief 设置header组件，header组件会加到scrollview的顶部
   * @param header 组件
   */
  void SetHeader(const std::shared_ptr<ScrollViewHeader> &header);

  /**
   * @brief Header的占位高度发生变化时候需要通过此方法更新ScrollRect的Top位置
   * @param height 高度
   */
  void UpdateHeaderHeight(int height);

  /**
   * @brief 返回Header
   * @return 组件
   */
  const std::shared_ptr<ScrollViewHeader> &GetHeader() const { return header_; }

  /**
   * @brief 停止滚动
   */
  void StopScrolling();

  /**
   * @brief 获取是否正在滑动
   * @return 是否正在滑动
   */
  constexpr bool IsScrolling() const { return scroll_started_; }

  /**
   * @brief 获取是否在惯性滑动
   * @return 是否在惯性滑动
   */
  bool IsFling() const;

  /**
   * @brief 获取是否在被拖动
   * @return 是否在被拖动
   */
  bool IsDragging() const;

 protected:
  enum RenderObjectIndex { kChildTransform = View::kRenderObjectCount, kTransformImpact, kRenderObjectCount };

  explicit ScrollView(const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent())
      : View(view_context) {}
  void Init() override;
  void Layout() override;
  int32_t GetTotalRenderObjectCount() override;
  void UpdateScaleIfNeeded(TScalar scale, TPoint focal_point);
  void InternalSetFrame(const TRect &frame) override;

  /**
   * @brief 初始化和调整ScrollView方面时需要设置对应的DragGesture
   */
  void InitGestureEvent();
  void InitTapGesture();
  void InitDragGesture();
  void InitScaleGesture();
  void HandleDragStartEvent(const tdfcore::DragStartDetails &details);
  void HandleDragUpdateEvent(const tdfcore::DragUpdateDetails &details);
  void HandleDragEndEvent(const tdfcore::DragEndDetails &details);
  void HandleDragCancelEvent(const tdfcore::DragCancelDetails &details);
  void HandleScaleStartEvent(const tdfcore::ScaleStartDetails &details);
  void HandleScaleUpdateEvent(const tdfcore::ScaleUpdateDetails &details);
  void HandleScaleEndEvent(const tdfcore::ScaleEndDetails &details);

  /**
   * @brief 任何原因导致的滚动开始时都会调用该方法，子类可重写该方法
   */
  virtual void OnScrollStart() {}

  /**
   * @brief 任何原因导致的滚动offset更新会调用该方法，子类可重写该方法
   * @param old_offset 旧的偏移量 new_offset 新的偏移量
   */
  virtual void OnScrollUpdate(TPoint old_offset, TPoint new_offset) {}

  /**
   * @brief 任何原因导致的滚动结束会调用该方法，子类可重写该方法
   */
  virtual void OnScrollEnd() {}

  /**
   * @brief 拖拽开始时调用该方法，子类可重写该方法
   */
  virtual void OnDragStart() {}

  /**
   * @brief 拖拽时offset更新会调用该方法，子类可重写该方法
   * @param old_offset 旧的偏移量 new_offset 新的偏移量
   */
  virtual void OnDragUpdate(TPoint old_offset, TPoint new_offset) {}

  /**
   * @brief 拖拽结束会调用该方法，子类可重写该方法
   */
  virtual void OnDragEnd() {}

  /**
   * @brief 拖拽取消时会调用该方法，子类可重写该方法
   */
  virtual void OnDragCancel() {}

  /**
   * @brief 拖拽手势结束后，惯性滚动开始时调用该方法，子类可重写该方法
   */
  virtual void OnFlingStart() {}

  /**
   * @brief 惯性滚动时offset更新会调用该方法，子类可重写该方法
   * @param old_offset 旧的偏移量 new_offset 新的偏移量
   */
  virtual void OnFlingUpdate(TPoint old_offset, TPoint new_offset) {}

  /**
   * @brief 惯性滚动结束会调用该方法，子类可重写该方法
   */
  virtual void OnFlingEnd() {}

  /**
   * @brief 缩放开始时会调用该方法，子类可重写该方法
   */
  virtual void OnScaleStart() {}

  /**
   * @brief 缩放更新时会调用该方法，子类可重写该方法
   */
  virtual void OnScaleUpdate(double current_scale) {}

  /**
   * @brief 缩放结束时会调用该方法，子类可重写该方法
   */
  virtual void OnScaleEnd() {}

  virtual bool UpdateOffset(const TPoint &offset);

  void AdjustScrollSize(int header_height = 0);
  void SetOffsetWithOverScroll(TPoint offset);
  void SetOffsetWithoutOverScroll(TPoint offset);
  virtual void CalculateMatrixAndApply(TPoint offset, TScalar scale);
  void SetOffsetWithPhysicsIfNeeded(TPoint offset);
  bool GetIsAnimating() const;
  TScalar min_scale_size_ = 0.25;
  TScalar max_scale_size_ = 16;
  TScalar current_scale_ = 1.0;
  TScalar delta_scale_ = 1.0;
  TM33 total_matrix_;
  TRect content_rect_ = TRect::MakeEmpty();
  ScrollDirection scroll_direction_ = ScrollDirection::kBoth;

 private:
  void HandleScrollEnd();
  void InitAnimationIfNeeded(TPoint old_offset, TPoint offset);

  static constexpr TDuration kAnimationDuration = std::chrono::milliseconds(250);
  TPoint offset_ = TPoint::Make(0, 0);
  TPoint current_velocity_ = {0, 0};
  TPoint scale_focal_point_;
  TPoint scale_pending_point_;
  TPoint scale_local_point_;
  bool scale_pending_ = false;
  TScalar prior_scale_ = 1;
  TPoint scale_start_focal_point_ = TPoint::Make(0, 0);
  bool scroll_started_ = false;
  bool is_animating_ = false;
  bool drag_started_ = false;
  bool fling_started_ = false;
  bool member_should_accepted_ = false;
  bool pointer_down_ = false;
  std::shared_ptr<TransformObject> child_transform_;
  std::shared_ptr<RenderObject> transform_impact_;
  std::shared_ptr<Ticker> physics_ticker_;
  std::function<void(TDuration)> ticker_call_back_;
  std::shared_ptr<ScrollPhysics> scroll_physics_;
  std::shared_ptr<AnimationController> animation_controller_;

  std::shared_ptr<TapGestureRecognizer> tap_gesture_recognizer_;
  std::shared_ptr<ScrollGestureRecognizer> scroll_gesture_recognizer_;
  std::shared_ptr<PinchRotateGestureRecognizer> scale_gesture_recognizer_;
  std::shared_ptr<ScrollViewHeader> header_;
  NoArgListener scroll_start_listener_;
  Listener<TPoint, TPoint> scroll_update_listener_;
  NoArgListener scroll_end_listener_;

  NoArgListener drag_start_listener_;
  Listener<TPoint, TPoint> drag_update_listener_;
  NoArgListener drag_end_listener_;

  NoArgListener fling_start_listener_;
  Listener<TPoint, TPoint> fling_update_listener_;
  NoArgListener fling_end_listener_;

  NoArgListener scale_start_listener_;
  Listener<double> scale_update_listener_;
  NoArgListener scale_end_listener_;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ScrollView, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::ScrollView)
