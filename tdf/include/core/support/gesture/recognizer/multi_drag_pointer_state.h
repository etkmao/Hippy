//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/constants.h"
#include "core/support/gesture/recognizer/drag_detail.h"
#include "core/support/gesture/recognizer/multi_drag_gesture_recognizer.h"

namespace tdfcore {
/**
 * 在多指手势中处理单个手指的滑动，与[MultiDragGestureRecognizer]配合使用，主要处理Move、Up、Cancel事件。
 * 这是一个抽象类，因为监听不同的滑动方向时竞技成功的条件不一样，具体请参考子类：
 * [ImmediatePointerState]
 * [HorizontalPointerState]
 * [VerticalPointerState]
 * [DelayedPointerState]
 * */
class MultiDragPointerState : public Object {
 public:
  explicit MultiDragPointerState(PointerID pointer, TPoint initial_position);

  void SetRecognizer(std::shared_ptr<MultiDragGestureRecognizer> recognizer) { recognizer_ = recognizer; }

  /**
   * 处理手势的Move类型事件，会根据滑动距离来判断是否满足竞技条件，如果满足触发回调
   * @params event - 手势详细数据
   * */
  void Move(const PointerData &data);
  /**
   * 手势竞技获胜后会触发，主要用于事件回调，通知监听者有滑动事件
   * @params client - 监听者，使用方式详见[Drag]
   * */
  void StartDrag(std::shared_ptr<tdfcore::Drag> client);
  /**
   * 手指滑动结束，离开屏幕时触发
   * */
  void Up();
  /**
   * 手指滑动过程发生中断时触发
   * */
  void Cancel();
  /**
   * [MultiDragGestureRecognizer]在竞技场中获胜时触发
   * */
  virtual void Accepted(GestureMultiDragStartCallback starter) = 0;
  /**
   * [MultiDragGestureRecognizer]在竞技场中失败时触发
   * */
  void Rejected();

  /**
   * 表示后续不再处理手势事件
   * */
  virtual void Dispose();

 protected:
  void Reject() const;
  void Accept() const;
  /**
   * 来用判断在滑动的过程是否可以竞技胜出，不同方向的手势阈值不一样，由子类控制
   * */
  virtual void CheckForResolutionAfterMove() = 0;

 protected:
  PointerID pointer_;
  TPoint pending_delta_;
  TPoint initial_position_;
  std::shared_ptr<VelocityTracker> velocity_tracker_;
  std::shared_ptr<tdfcore::Drag> client_;
  TDuration last_pending_event_timestamp_;

  std::weak_ptr<MultiDragGestureRecognizer> recognizer_;
};

}  // namespace tdfcore
