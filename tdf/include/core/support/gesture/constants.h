//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/common/time.h"

namespace tdfcore {

constexpr TDuration kPressTimeout = TDuration(std::chrono::milliseconds(100));

constexpr TDuration kHoverTapTimeout = TDuration(std::chrono::milliseconds(150));

constexpr double kHoverTapSlop = 20.0;

constexpr TDuration kLongPressTimeout = TDuration(std::chrono::milliseconds(500));

constexpr TDuration kDoubleTapTimeout = TDuration(std::chrono::milliseconds(300));

const TDuration kDoubleTapMinTime = TDuration(std::chrono::milliseconds(40));

constexpr double kTouchSlop = 8.0;

constexpr double kDoubleTapTouchSlop = kTouchSlop;

constexpr double kDoubleTapSlop = 10.0;

constexpr TDuration kZoomControlsTimeout = TDuration(std::chrono::milliseconds(3000));

constexpr double kPagingTouchSlop = kTouchSlop * 2;

constexpr double kPanSlop = kTouchSlop;

constexpr double kScaleSlop = 4.0;

constexpr double kWindowTouchSlop = 16.0;

constexpr double kMinFlingVelocity = 50.0;

constexpr double kMaxFlingVelocity = 8000.0;  // Logical pixels / second

constexpr TDuration kJumpTapTimeout = TDuration(std::chrono::milliseconds(500));

constexpr double kDefaultPixelsPerSecond = 0;

}  // namespace tdfcore
