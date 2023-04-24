//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/constants.h"
#include "core/support/gesture/recognizer/drag_detail.h"
#include "core/support/gesture/recognizer/one_sequence_gesture_recognizer.h"
#include "core/support/gesture/recognizer/velocity_tracker.h"
#include "core/tdfi/timer.h"

namespace tdfcore {
extern Velocity velocity_zero;

enum class ScrollType {
  /// 响应 手指，触摸板，手写笔 滑动，鼠标滚轮，不响应鼠标点击拖拽
  kScroll,
  /// 响应 手指，触摸板，手写笔 滑动，不响应鼠标
  kDrag,
  /// 只响应滚轮
  kWheel,
};

enum class ScrollGestureState {
  kReady,
  kPossible,
  kDragAccepted,
  kCancel
};

class ScrollGestureRecognizer : public OneSequenceGestureRecognizer {
  TDF_REFF_CLASS_META(ScrollGestureRecognizer)
 public:
  ScrollGestureRecognizer() = default;

  /**
   * 设置最小滑动速度阈值
   * @param min_fling_velocity 最小滑动速度
   */
  constexpr void SetMinFlingVelocity(const double &min_fling_velocity) {
    min_fling_velocity_ = min_fling_velocity;
  }

  /**
   * 设置最小滑动距离阈值
   * @param min_fling_distance 最小滑动距离
   */
  constexpr void SetMinFlingDistance(const double &min_fling_distance) {
    min_fling_distance_ = min_fling_distance;
  }

  /**
   * 设置允许滑动的全局最小滑动距离
   * @param min_global_moved_distance 最小滑动距离
   */
  void SetMinGlobalMovedDistance(double min_global_moved_distance) {
    min_global_moved_distance_ = min_global_moved_distance;
  }

  /**
   * @brief 设置 down 回调
   * @param on_down 回调
   */
  void SetDown(const DragDownClosure &on_down) { on_down_ = on_down; }

  /**
   * @brief 设置 start 回调
   * @param on_start 回到
   */
  void SetStart(const DragStartClosure &on_start) { on_start_ = on_start; }

  /**
   * @brief 设置 update 回调
   * @param on_update 回调
   */
  void SetUpdate(const DragUpdateClosure &on_update) { on_update_ = on_update; }

  /**
   * @brief 设置 end 回调
   * @param on_end 回调
   */
  void SetEnd(const DragEndClosure &on_end) { on_end_ = on_end; }

  /**
   * @brief 设置 cancel 回调
   * @param on_cancel 回调
   */
  void SetCancel(const DragCancleClosure &on_cancel) { on_cancel_ = on_cancel; }

  /**
   * @brief 设置滑动类型
   * @param scroll_type 滑动类型
   */
  void SetScrollType(const ScrollType scroll_type) {
    scroll_type_ = scroll_type;
  }


 protected:
  double min_global_moved_distance_ = kPanSlop;

  bool CanAddPointer(const PointerData &data) const override;
  void AcceptGesture(PointerID pointer) override;
  void RejectGesture(PointerID pointer) override;
  void DidStopTrackingLastPointer(PointerID pointer) override;
  void HandlePointerDown(const PointerData &data) override;
  void HandlePointerMove(const PointerData &data) override;
  void HandlePointerUp(const PointerData &data) override;
  void HandlePointerScroll(const PointerData &data)  override;
  void HandlePointerCancel(const PointerData &data) override;

  void Init() override;

  /**
   * @brief 手势的拖拽移动距离是否大于系统设定的阈值
   *
   * @return true: 移动距离大于阈值 false: 移动距离小于阈值
   */
  bool HasSufficientGlobalDistanceToAccept() const;

  /**
   * @brief 判断是否是滑动手势
   *
   * @param estimate 速度评估器
   * @return true: 滑动手势 false: 非滑动手势
   */
  virtual bool IsFlingGesture(const VelocityEstimate &estimate) const = 0;

  /**
   * @brief 获取相对于上一个事件的滑动位移的增量
   *
   * @param delta 增量值
   * @return 位移增量的坐标值
   */
  virtual TPoint GetDelta(const TPoint &delta) const = 0;

  /**
   * 获取相对于上一个事件沿主坐标轴方向的滑动位移增量
   * @param value 数值
   * @return 位移增量
   */
  virtual double GetPrimaryValue(const TPoint &value) const = 0;

  void CheckDown(PointerID pointer);
  void CheckStart(PointerID pointer, const TDuration &timeStamp);
  void CheckUpdate(const DragUpdateDetails &updateDetails);
  void CheckEnd(PointerID pointer);
  void CheckCancel(PointerID pointer);

  double global_distance_moved_ = 0;
  double min_fling_velocity_ = kMinFlingVelocity;
  double min_fling_distance_ = kTouchSlop;
  ScrollGestureState state_ = ScrollGestureState::kReady;

 private:
  size_t FindPointerData(PointerID pointer);
  TPoint GetMultiPointerDelta() const;
  TPoint GetAveragePositon() const;
  void CheckMultiPointerUpdate();

  std::vector<PointerData> multi_moved_data_;
  ScrollType scroll_type_ = ScrollType::kDrag;
  double max_fling_velocity_{kMaxFlingVelocity};

  TPoint initial_position_{};
  TPoint pending_drag_offset_{};
  TDuration last_pending_event_timestamp_{};

  std::map<PointerID, std::shared_ptr<VelocityTracker>> velocity_trackers_;
  TPoint up_position_{};
  TPoint cancel_position_{};
  std::vector<PointerData> pointer_data_vector_;

  DragDownClosure on_down_;
  DragStartClosure on_start_;
  DragUpdateClosure on_update_;
  DragEndClosure on_end_;
  DragCancleClosure on_cancel_;
};

}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::ScrollGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::ScrollGestureRecognizer)