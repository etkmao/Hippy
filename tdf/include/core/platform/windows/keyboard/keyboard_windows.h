//
// Created by winctoryang on 2021/11/29.
//

#pragma once
#include "core/platform/common/clipboard.h"
#include "core/platform/common/keyboard/keyboard.h"
#include "core/platform/common/text_input_model.h"
#include "core/platform/windows/windows_define.h"

namespace tdfcore {
class KeyboardWindows final : public Keyboard {
 public:
  using UpdateCaretHandler = std::function<void(const TRect& caret_rect)>;
  void Init() override;
  void Show() override;
  void Hide() override;
  void SetKeyboardConfiguration(int32_t client_id, const KeyboardConfiguration& keyboard_configuration) override;
  void SetEditingState(const EditingState& editing_state) override;
  void SetCaretRect(const TRect& caret_rect) override;
  void SetCaretUpdateHandler(const UpdateCaretHandler& handler) { update_caret_handler_ = handler; }

 protected:
  void OnKeyDown(const std::shared_ptr<KeyboardEvent>& event) override;

 private:
  KeyboardInputType input_type_;
  KeyboardAction action_;
  UpdateCaretHandler update_caret_handler_;
};
}  // namespace tdfcore
