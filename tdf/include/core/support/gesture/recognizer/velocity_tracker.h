//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <list>
#include <vector>
#include "core/common/object.h"
#include "core/common/time.h"
#include "core/common/point.h"

namespace tdfcore {

class Velocity {
 public:
  Velocity() = default;
  explicit Velocity(const TPoint &pixelsPerSecond) : pixels_per_second_(pixelsPerSecond) {}

  /**
   * 根据上下限值，重新计算速度
   * @param minValue 下限值
   * @param maxValue 上限值
   * @return 新的速度
   */
  Velocity ClampMagnitude(double minValue, double maxValue);
  Velocity operator-() const;
  Velocity operator-(const Velocity &other) const;
  Velocity operator+(const Velocity &other) const;
  bool operator==(const Velocity &other) const;

  /**
   * @brief 获取 pixels_per_second_
   * @return pixels_per_second_
   */
  constexpr TPoint GetPixelsPerSecond() const { return pixels_per_second_; }

 private:
  TPoint pixels_per_second_ = TPoint::Make(0, 0);
};

struct VelocityEstimate {
 public:
  TPoint pixels_per_second;
  double confidence;
  TDuration duration;
  TPoint offset;

  VelocityEstimate() : confidence(-1) {}
};

struct PointAtTime {
 public:
  TPoint point;
  TDuration point_time;

  PointAtTime() {}
  PointAtTime(const TPoint &point, const TDuration &time) : point(point), point_time(time) {}
};

class VelocityTracker : public Object {
 public:
  virtual ~VelocityTracker() = default;
  static constexpr TDuration assume_pointer_move_stopped_microseconds = TDuration(std::chrono::milliseconds(40));
  static constexpr int history_size = 20;
  static constexpr TDuration horizon_microseconds = TDuration(std::chrono::milliseconds(50));
  static constexpr int min_sample_size = 3;

  /**
   * 在当前time时刻往当前速度追踪器中添加一个位置
   * @param time
   * @param position
   */
  void AddPosition(TDuration time, TPoint position);

  /**
   * 获取当前被追踪实体的速度评估器，评估器中包好了一些对当前速度追踪器可用的信息。
   * @return
   */
  VelocityEstimate GetVelocityEstimate() const;

  /**
   * 获取当前的 Velocity
   * @return
   */
  Velocity GetVelocity();

 protected:
  VelocityTracker() = default;

 private:
  std::vector<PointAtTime> samples_;
  int index_ = 0;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
