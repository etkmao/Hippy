//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <emscripten/val.h>
#include "core/platform/common/channel/host_object.h"
#include "core/platform/common/keyboard/keyboard.h"
#include "core/platform/common/keyboard/keyboard_constants.h"
#include "core/platform/common/text_input_model.h"
#include "core/tdfi/event.h"

namespace tdfcore {

class KeyboardWeb final : public Keyboard {
 public:
  KeyboardWeb();
  void Show() override;
  void Hide() override;
  void SetEditingState(const EditingState& editing_state) override;
  void SetKeyboardConfiguration(int32_t id, const KeyboardConfiguration& keyboard_configuration) override;
  void SetCaretRect(const TRect& caret_rect) override;

  /**
   * @brief 文本输入事件回调
   * @param event_type 事件类型
   * @param data 文本数据
   */
  void OnComposition(const std::string& event_type, const std::string& data);

  /**
   * @brief 字符输入
   * @param data
   */
  void OnCharacterInput(const std::string& data);

 protected:
  void OnKeyDown(const std::shared_ptr<KeyboardEvent>& event) override;
  void InsertNewLine();

 private:
  KeyboardInputType input_type_;
  KeyboardAction action_;
  emscripten::val text_editing_ = emscripten::val::null();
};

}  // namespace tdfcore
