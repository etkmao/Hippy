//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include "core/common/object.h"
#include "core/platform/web/keyboard/keyboard_web.h"
#include "core/tdfi/shell.h"

namespace tdfcore {

/**
 * @brief Web平台键盘、鼠标、屏幕事件处理派发
 */
class PlatformEventHandler : public Object {
 public:
  ~PlatformEventHandler() override;

  /**
   * @brief 处理（由web传递过来的）drag事件
   * @param engine_ptr 这里是web传过来的engine指针地址
   * @param drag_event 事件数据
   */
  static void HandleDragEvent(intptr_t engine_ptr, const emscripten::val &drag_event);

 protected:
  /**
   * @brief 构造函数
   * @param shell Shell实例
   * @param target_html_element_id 监听指定的HTML元素的事件（一般为TDFOutputView所对应的HTML元素）
   */
  PlatformEventHandler(const std::weak_ptr<Shell> &shell, const std::string &target_html_element_id);

 private:
  struct EmscriptenEventCallbacks {
    em_mouse_callback_func mouse = nullptr;
    em_wheel_callback_func wheel = nullptr;
    em_touch_callback_func touch = nullptr;
    em_key_callback_func keyboard = nullptr;
  };

 private:
  std::weak_ptr<Shell> shell_;
  std::string target_html_element_id_;

  void RegisterEmscriptenEventCallbacks(const EmscriptenEventCallbacks &callbacks);
  static EM_BOOL HandleMouseEvent(int event_type, const EmscriptenMouseEvent *event, void *user_data);
  static EM_BOOL HandleWheelEvent(int event_type, const EmscriptenWheelEvent *event, void *user_data);
  static EM_BOOL HandleTouchEvent(int event_type, const EmscriptenTouchEvent *event, void *user_data);
  static EM_BOOL HandleKeyboardEvent(int event_type, const EmscriptenKeyboardEvent *event, void *user_data);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
