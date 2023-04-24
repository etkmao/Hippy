//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/platform/common/keyboard/keyboard.h"
#include "core/support/text/text_base.h"
#import <Cocoa/Cocoa.h>

@class TDFTextInputPlugin;
namespace tdfcore {

class KeyboardMacOS final : public Keyboard {
 public:
  void Init() override;
  void Show() override;
  void Hide() override;
  void SetKeyboardConfiguration(int32_t client_id, const KeyboardConfiguration& keyboard_configuration) override;
  void SetEditingState(const EditingState &editing_state) override;
  void SetCaretRect(const TRect& caret_rect) override;
 private:
  TDFTextInputPlugin *textInputPlugin_;
};
}  // namespace tdfcore

#pragma mark - TDFTextInputPlugin

/// 处理文本输入的插件，内核通过这个类连接原生文本输入。这个类重新实现了NSTextInputClient协议
@interface TDFTextInputPlugin : NSTextView

- (instancetype)initWithDelegate:(std::weak_ptr<tdfcore::TextInputDelegate>)delegate;

/**
 * @brief 激活键盘
 * @param caretRect 输入光标基于屏幕的rect
 */
- (void)showTextInputWithCaretRect:(NSRect)caretRect;

/**
 * @brief 设置光标位置
 * @param caretRect 输入光标基于屏幕的rect
 */
- (void)setCaretRect:(NSRect)caretRect;

/**
 * @brief 使键盘无效
 */
- (void)hideTextInput;

/**
 * @brief 设置键盘配置和当前键盘使用者的id，内核用client id来记录当前操作键盘的组件标识
 * @param client_id 记录当前键盘使用者的标识
 * @param configuration 键盘配置
 */
- (void)setTextInputClient:(int32_t)client_id withConfiguration:(const tdfcore::KeyboardConfiguration &)configuration;

/**
 * @brief 设置编辑信息
 */
- (void)setTextInputEditingState:(const tdfcore::EditingState &)editing_state;

/**
 * @brief 处理指定的键盘事件
 */
- (BOOL)handleKeyEvent:(NSEvent *)event;
@end
