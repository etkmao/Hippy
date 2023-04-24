//
// Created by winctoryang on 2021/11/29.
//

#pragma once

#include <unordered_map>
#include "core/platform/windows/windows_define.h"
#include "core/tdfi/event.h"
#include "core/tdfi/shell.h"

namespace tdfcore {

enum class PointerDevice : int64_t { kClick, kScroll, kScale, kHover };

/**
 * @brief 处理平台侧系统事件
 * @details
 *   使用方式：
 *   请将本类提供的HandleMessage()插入到消息主循环处理链中，凡是本类能够处理的消息，该方法将返回true，否则将返回false；
 *   使用者可根据HandleMessage()的返回值，决定自己进一步的处理逻辑
 */
class PlatformEventHandler : public Object {
 public:
  ~PlatformEventHandler() override = default;

  /**
   * @brief 处理系统消息
   * @param message 消息类型
   * @param wparam 消息的WPARAM参数
   * @param lparam 消息的LPARAM参数
   * @param window_handle 处理消息循环的主窗口的句柄
   * @return 是否有处理本条消息
   */
  bool HandleMessage(UINT const message, WPARAM const wparam, LPARAM const lparam, HWND window_handle);

 protected:
  explicit PlatformEventHandler(const std::weak_ptr<Shell> &shell);

 private:
  std::weak_ptr<Shell> shell_;
  bool is_tracking_mouse_leave_ = false;
  char32_t pending_high_surrogate_ = 0;
  std::unordered_map<KeyboardKey, std::string> pressing_characters_;

  void HandleMouseButtonMessage(WPARAM const wparam, LPARAM const lparam, HWND window_handle, PointerType type,
                                PointerButtonType button);
  void HandleMouseMoveMessage(WPARAM const wparam, LPARAM const lparam, HWND window_handle, PointerType type);
  void HandleMouseWheelMessage(WPARAM const wparam, LPARAM const lparam, HWND window_handle, bool horizontal);
  void HandleTouchMessage(WPARAM const wparam, LPARAM const lparam, HWND window_handle);
  void HandleKeyDownMessage(WPARAM const wparam, LPARAM const lparam, HWND window_handle);
  void HandleKeyUpMessage(WPARAM const wparam, LPARAM const lparam);
  PointerButtonType GetPointerButton(WPARAM const wparam);
  TPoint GetPointerPosition(LPARAM const lparam, HWND window_handle, bool is_screen_location);
  KeyboardKey GetKeyboardKey(WPARAM const wparam, LPARAM const lparam);
  std::string GetCharacter(WPARAM const wparam, LPARAM const lparam, HWND window_handle);
  ModifierFlags GetCurrentModifierFlags();
  void SendPointerEvent(const std::vector<PointerData> &pointer_data_list);
  void SendKeyboardEvent(KeyboardEvent::EventType type, KeyboardKey key, const std::string &character);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
