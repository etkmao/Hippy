//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/common/time.h"
#include "core/support/gesture/event/pointer_data.h"
#include "core/support/gesture/recognizer/velocity_tracker.h"

namespace tdfcore {

struct DragDetails {
  /// 全局坐标系
  TPoint position{TPoint()};
  PointerButtonType button_type = PointerButtonType::kPrimary;
  DeviceKind kind = DeviceKind::kTouch;
};

struct DragDownDetails : public DragDetails {};

struct DragStartDetails : public DragDetails {
  TDuration source_timestamp;
};

struct DragUpdateDetails : public DragDetails {
  TDuration source_timestamp;
  TPoint delta{TPoint()};
  double primary_delta;
};

struct DragEndDetails : public DragDetails {
  Velocity velocity;
  double primary_velocity;

  DragEndDetails() = default;
  DragEndDetails(Velocity velocity, double primaryVelocity) : velocity(velocity), primary_velocity(primaryVelocity) {}
};

struct DragCancelDetails : public DragDetails {
  DragCancelDetails() = default;
};

/**
 * 在[MultiDragGestureRecognizer]中处理单个手指滑动的回调
 * */
class Drag : public Object {
 public:
  /**
   * 手指正在滑动
   * @params details - 手指位置详细信息
   * */
  virtual void Update(DragUpdateDetails details) = 0;

  /**
   * 手指滑动结束
   * @params details - 手指位置详细信息
   * */
  virtual void End(DragEndDetails details) = 0;

  /**
   * 手指滑动取消
   * */
  virtual void Cancel() = 0;
};

using GestureMultiDragStartCallback = std::function<std::shared_ptr<Drag>(const TPoint &position)>;

using DragDownClosure = std::function<void(const DragDownDetails &details)>;
using DragStartClosure = std::function<void(const DragStartDetails &details)>;
using DragUpdateClosure = std::function<void(const DragUpdateDetails &details)>;
using DragEndClosure = std::function<void(const DragEndDetails &details)>;
using DragCancleClosure = std::function<void(const DragCancelDetails &details)>;
}  // namespace tdfcore
