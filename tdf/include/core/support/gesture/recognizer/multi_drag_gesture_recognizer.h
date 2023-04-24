//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <map>
#include <memory>
#include <set>
#include <utility>

#include "core/support/gesture/constants.h"
#include "core/support/gesture/event/pointer_data.h"
#include "core/support/gesture/recognizer/drag_detail.h"
#include "core/support/gesture/recognizer/gesture_recognizer.h"

namespace tdfcore {
class MultiDragPointerState;

/**
 * @brief 监听多指手势滑动事件，此类为抽象父类，使用其子类
 * [ImmediateMultiDragGestureRecognizer]
 * [HorizontalMultiDragGestureRecognizer]
 * [VerticalMultiDragGestureRecognizer]
 * [DelayedMultiDragGestureRecognizer]
 * 其中Recognizer处理多个手指时，每个手指会对应一个[MultiDragPointerState]，事件的处理、竞技获胜
 * 条件都由[MultiDragPointerState]控制
 */
class MultiDragGestureRecognizer : public GestureRecognizer {
  TDF_REFF_CLASS_META(MultiDragGestureRecognizer)
 public:
  MultiDragGestureRecognizer() = default;

  void Reject(PointerID pointer);
  void Accept(PointerID pointer);

  /**
   * 监听回调，当有新的手指滑动时触发调用on_start，详细信息详见[GestureMultiDragStartCallback]
   * 其中[GestureMultiDragStartCallback]回调参数只有位置信息position，没有标识是哪只手指，所以
   * 调用方要保证on_start返回值[Drag]是每只手指独有
   * */
  void SetStartCallback(const GestureMultiDragStartCallback& on_start);
  ~MultiDragGestureRecognizer() override = default;

 protected:
  void AcceptGesture(PointerID initial_position) override;
  void RejectGesture(PointerID pointer) override;
  void HandlePointerDown(const PointerData &data) override;
  void HandlePointerMove(const PointerData &data) override;
  void HandlePointerUp(const PointerData &data) override;
  void HandlePointerCancel(const PointerData &data) override;
  /**
   * 为每个手指创建对应的[MultiDragPointerState]，因为不同的滑动方向条件不一样，需要由子类来指定
   * */
  virtual std::shared_ptr<MultiDragPointerState> CreateNewPointerState(const PointerData &data) const = 0;
  void RemoveState(PointerID pointer);
  std::shared_ptr<Drag> StartDrag(TPoint initial_position, PointerID pointer);

 private:
  GestureMultiDragStartCallback on_start_;
  std::map<PointerID, std::shared_ptr<MultiDragPointerState>> pointers_;

  bool IsTrackingPointer(PointerID pointer) const;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::MultiDragGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::MultiDragGestureRecognizer)