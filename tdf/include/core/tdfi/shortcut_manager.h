//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/object.h"
#include "core/tdfi/event_center.h"

namespace tdfcore {

/**
 * @brief 统一管理快捷事件，例如键盘的快捷键、移动端文字编辑菜单的快捷操作等，将产生TDF统一的快捷事件
 */
class ShortcutManager : public Object, public std::enable_shared_from_this<ShortcutManager> {
 public:
  ~ShortcutManager() override;
  void Init() override;

  /**
   * @brief 手动发送快捷事件
   * @param shortcut 快捷事件类型
   */
  // TODO(dexterzhao): KeyboardAction这个名字起的不好，以后再改吧
  void SendShortcut(KeyboardAction shortcut);

 protected:
  ShortcutManager(const std::shared_ptr<EventCenter> &event_center, const std::shared_ptr<EngineClock> &clock);

 private:
  std::shared_ptr<EventCenter> event_center_;
  std::shared_ptr<EngineClock> clock_;
  uint64_t keyboard_event_listener_;
  ModifierFlag edit_modifier_flag_ = ModifierFlag::kControl;

  void AddEventListeners();
  void RemoveEventListeners();
  void HandleEvent(const std::shared_ptr<KeyboardEvent> &event);
  KeyboardAction GetShortcutFromKeyboardEvent(const std::shared_ptr<KeyboardEvent> &event);

  template <typename T>
  inline uint64_t AddEventListener() {
    return event_center_->AddListener(T::ClassType(), [WEAK_THIS](std::shared_ptr<Event> event, uint64_t id) {
      DEFINE_SELF(ShortcutManager);
      if (self) {
        self->HandleEvent(std::static_pointer_cast<T>(event));
      }
      return EventDispatchBehavior::kContinue;
    });
  }

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
