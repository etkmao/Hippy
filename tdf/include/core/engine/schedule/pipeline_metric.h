//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <list>
#include <ostream>

#include "core/common/time.h"

namespace tdfcore {
/**
 * @brief Pipeline子阶段时间点
 */
class PipelineTiming {
 public:
  /// 开始时间
  RealTimePoint begin;

  /// 结束时间
  RealTimePoint end;

  PipelineTiming() = default;

  void Set(RealTimePoint begin, RealTimePoint end) {
    this->begin = begin;
    this->end = end;
  }

  friend std::ostream &operator<<(std::ostream &out, PipelineTiming &timing) {
    out << timing.begin.time_since_epoch().count() << " ---> "
        << timing.end.time_since_epoch().count() << ", " << (timing.end - timing.begin).count();
    return out;
  }

  bool IsValid() const {
    return begin.time_since_epoch().count() > 0 && begin.time_since_epoch().count() > 0;
  }
};

/**
 * @brief 渲染过程的时间点
 */
class FrameTiming {
 public:
  /// Layout流程的时间点
  PipelineTiming layout_timing;

  /// Paint流程的时间点
  PipelineTiming paint_timing;

  /// Render流程的时间点
  PipelineTiming raster_timing;

  FrameTiming() = default;

  /**
   * @brief UI过程的耗时
   * 若Layout、Paint都执行: 耗时 = Paint结束时间 - Layout开始时间
   * 若只执行Layout: 耗时 = Layout开始时间 - Layout结束时间
   * 若只执行Paint: 耗时 = Paint开始时间 - Paint结束时间
   * @return Duration
   */
  TDuration GetUIDelta() const {
    if (!layout_timing.IsValid() && !paint_timing.IsValid()) {
      return TDuration::zero();

    } else if (layout_timing.IsValid() && !paint_timing.IsValid()) {
      return layout_timing.end - layout_timing.begin;

    } else if (!layout_timing.IsValid() && paint_timing.IsValid()) {
      return paint_timing.end - paint_timing.begin;

    } else {
      return paint_timing.end - layout_timing.begin;
    }
  }

  /**
   * @brief UI过程的时间点
   * 若Layout、Paint都执行: [开始时间 = Layout开始时间] [结束时间 = Paint结束时间]
   * 若只执行Layout: [开始时间 = Layout开始时间] [结束时间 = Layout结束时间]
   * 若只执行Paint: [开始时间 = Paint开始时间] [结束时间 = Paint结束时间]
   * @return PipelineTiming
   */
  PipelineTiming GetUITiming() const {
    PipelineTiming timing;
    if (!layout_timing.IsValid() && !paint_timing.IsValid()) {
      return timing;

    } else if (layout_timing.IsValid() && !paint_timing.IsValid()) {
      return layout_timing;

    } else if (!layout_timing.IsValid() && paint_timing.IsValid()) {
      return paint_timing;

    } else {
      timing.Set(layout_timing.begin, paint_timing.end);
      return timing;
    }
  }

  /// 光栅化过程的时间点
  PipelineTiming GetRasterTiming() const { return raster_timing; }

  /// 光栅化过程的耗时
  TDuration GetRasterDelta() const { return raster_timing.end - raster_timing.begin; }
};

/**
 * @brief FrameTiming集合
 */
class FrameTimings {
 public:
  FrameTimings() = default;

  std::list<FrameTiming> list;

  /**
   * @brief 转Json
   */
  std::string ToJson() const;
};
}  // namespace tdfcore
