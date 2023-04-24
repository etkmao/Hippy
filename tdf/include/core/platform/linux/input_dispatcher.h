//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <linux/input.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include "core/common/listener.h"
#include "core/common/object.h"
#include "core/platform/common/viewport_metrics.h"
#include "core/platform/linux/platform_linux.h"
#include "core/support/gesture/event/pointer_data.h"

namespace tdfcore {

using PointerDataUpdateListener = std::function<void(PointerData)>;
/**
 * @brief Linux平台，键盘、鼠标、触摸板的事件分发接口
 */
class InputDispatcher : public Object {
 public:
  static std::shared_ptr<InputDispatcher> Make(WindowType type = WindowType::kNone);

  virtual void StartDispatch() = 0;

  virtual void StopDispathch() = 0;

  uint64_t AddListener(PointerDataUpdateListener listener) { return input_listener_.Add(listener); }

  void RemoveListener(uint64_t id) { input_listener_.Remove(id); }

  void UpdateViewport(ViewportMetrics metrics) { viewport_metrics_ = metrics; }

 protected:
  void Notify(PointerData data) { input_listener_.Notify(data); }
  ViewportMetrics viewport_metrics_;

 private:
  Listener<PointerData> input_listener_;
};

/**
 * @brief 基于epoll和dev/input的默认实现，不基于任何窗口系统。
 *        缺点是需要自己额外开发（因为不基于任何窗口系统）：
 *           1. 维护屏幕绝对坐标等信息
 *           2. 基于原始信息组装抽象事件（点击、移动等）
 *           3. 窗口内判断，焦点判断
 *        此外，由于监听了dev/input，需要在 sudo 模式下运行。
 *
 *        TODO(vimerzhao): 把通用逻辑提取出来，作为一个委派类被持有
 */
class InputDispatcherImpl : public InputDispatcher {
 public:
  ~InputDispatcherImpl() override { StopDispathch(); }

  void StartDispatch() override;

  void StopDispathch() override;

  void Listen();

 private:
  /**
   * @brief https://www.kernel.org/doc/Documentation/input/event-codes.txt
   */
  void HandleInputEvent(int fd);

  // EV_KEY: Used to describe state changes of keyboards, buttons, or other key-like devices.
  // TODO(vimerzhao)

  // EV_REL: Used to describe relative axis value changes, e.g. moving the mouse 5 units to the left.
  void HandleRelEvent(const input_event &event);

  // EV_ABS: Used to describe absolute axis value changes, e.g. describing the coordinates of a touch on a touchscreen.
  void HandleAbsEvent(const input_event &event);

  int epoll_fd_ = 0;

  // 和键盘、鼠标事件不同：一个完整的触摸事件是一系列 input_event
  std::vector<input_event> touch_events_;
};

/**
 * @brief 基于X11的API实现
 */
class InputDispatcherX11Impl : public InputDispatcher {};

/**
 * @brief 基于Wayland的API实现
 */
class InputDispatcherWaylandImpl : public InputDispatcher {};
}  // namespace tdfcore
