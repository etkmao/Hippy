//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <optional>
#include "core/support/gesture/recognizer/velocity_tracker.h"
#include "core/support/gesture/recognizer/one_sequence_gesture_recognizer.h"

namespace tdfcore {
/**
 * 识别器状态枚举
 */
enum class ScaleState {
  // 识别器已准备好开始识别手势
  kReady,
  // 迄今为止检测到的事件与 Scale 手势一致，但还未明确接收
  kPossible,
  // 明确接受为 Scale 手势
  kAccepted,
  // 明确接受为 Scale 手势，开始建立 Scale 手势焦点和初始 Scale 比例
  kStarted,
};

/**
 * Scale 手势 Start 回调参数结构
 */
struct ScaleStartDetails {
  // 与屏幕接触初始焦点坐标，坐标系为 全局坐标系
  TPoint focal_point;
  // 与屏幕接触初始焦点坐标，坐标系为 响应的 RenderObject 的本地坐标系
  TPoint local_focal_point;
  // 识别器跟踪的手指数量
  int64_t pointer_count = 0;
};

/**
 * Scale 手势 Update 回调参数结构
 */
struct ScaleUpdateDetails {
  // 跟上一次 update 事件的差值
  TPoint delta;
  // 当前 update 的焦点坐标，坐标系为 全局坐标系
  TPoint focal_point;
  //  当前 update 的焦点坐标，坐标系为 响应的 RenderObject 的本地坐标系
  TPoint local_focal_point;
  // 缩放比例，值大于等于 0
  double scale = 0;
  // 水平缩放比例
  double horizontal_scale = 0;
  // 垂直缩放比例
  double vertical_scale = 0;
  // 与屏幕接触时两个手指的旋转角度
  double rotation = 0;
  // 识别器跟踪的手指数量
  int64_t pointer_count = 0;
};

/**
 * Scale 手势 End 回调参数结构
 */
struct ScaleEndDetails {
  // 最后一个 pointer 离开屏幕的速度
  Velocity velocity;
  // 识别器跟踪的手指数量
  int64_t pointer_count = 0;
};

using GestureScaleStartCallback = std::function<void(const ScaleStartDetails &details)>;
using GestureScaleUpdateCallback = std::function<void(const ScaleUpdateDetails &details)>;
using GestureScaleEndCallback = std::function<void(const ScaleEndDetails &details)>;

/**
 * 抽象定义两个 pointer 之间为一条线，用于跟 Scale 手势的旋转
 */
struct LineBetweenPointers {
  // 标记 pointer 的起始位置
  TPoint pointer_start_location = TPoint::Make(0, 0);
  int64_t pointer_start_id = 0;

  // 标记 pointer 的结束位置
  TPoint pointer_end_location = TPoint::Make(0, 0);
  int64_t pointer_end_id = 1;

  friend bool operator==(const LineBetweenPointers &lhs, const LineBetweenPointers &rhs);
};

bool operator==(const LineBetweenPointers &lhs, const LineBetweenPointers &rhs);

/**
 * Scale 手势识别器
 */
class PinchRotateGestureRecognizer : public OneSequenceGestureRecognizer {
  TDF_REFF_CLASS_META(PinchRotateGestureRecognizer)
 public:
  /**
   * @brief 配置 start 状态回调
   * @param on_start 回调
   */
  void SetStart(const GestureScaleStartCallback &on_start) { on_start_ = on_start; }
  /**
   * @brief 配置 update 状态回调
   * @param on_update 回调
   */
  void SetUpdate(const GestureScaleUpdateCallback &on_update) { on_update_ = on_update; }
  /**
   * @brief 设置 end 状态回调
   * @param on_end 回调
   */
  void SetEnd(const GestureScaleEndCallback &on_end) { one_end_ = on_end; }

 protected:
  bool CanAddPointer(const PointerData &data) const override;
  void AcceptGesture(PointerID pointer) override;
  void RejectGesture(PointerID pointer) override;
  void HandlePointerDown(const PointerData &data) override;
  void HandlePointerMove(const PointerData &data) override;
  void HandlePointerUp(const PointerData &data) override;
  void HandlePointerCancel(const PointerData &data) override;
  void HandlePointerScale(const PointerData &data) override;

  void DidStopTrackingLastPointer(PointerID pointer) override;

 private:
  ScaleState state_ = ScaleState::kReady;
  TPoint initial_focal_point_;
  TPoint current_focal_point_;
  double initial_span_;
  double current_span_;
  double initial_horizontal_span_;
  double current_horizontal_span_;
  double initial_vertical_span_;
  double current_vertical_span_;
  std::optional<LineBetweenPointers> initial_line_;
  std::optional<LineBetweenPointers> current_line_;
  std::map<PointerID, TPoint> pointer_locations_;
  std::vector<PointerID> pointer_queue_;
  std::map<PointerID, std::shared_ptr<VelocityTracker>> velocity_trackers_;

  GestureScaleStartCallback on_start_;
  GestureScaleUpdateCallback on_update_;
  GestureScaleEndCallback one_end_;

  double GetScaleFactor() const { return initial_span_ > 0.0 ? current_span_ / initial_span_ : 1.0; }
  double GetHorizontalScaleFactor() const {
    return initial_horizontal_span_ > 0.0 ? current_horizontal_span_ / initial_horizontal_span_ : 1.0;
  }
  double GetVerticalScaleFactor() const {
    return initial_vertical_span_ > 0.0 ? current_vertical_span_ / initial_vertical_span_ : 1.0;
  }

  double ComputeRotationFactor();
  void UpdateLines();
  void Update();
  void InitValue();
  bool IsEnd(PointerID pointer);
  void AdvanceStateMachine(bool should_start_if_accepted);
  void DispatchOnStartIfNeeded();
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::PinchRotateGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::PinchRotateGestureRecognizer)