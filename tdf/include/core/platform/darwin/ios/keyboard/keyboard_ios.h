//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#import <UIKit/UIKit.h>
#include "core/platform/common/keyboard/keyboard.h"

@class TDFTextInputPlugin;
namespace tdfcore {

class KeyboardIOS final : public Keyboard {
 public:
  void Init() override;
  void Show() override;
  void Hide() override;
  bool IsShowing() override;
  void SetKeyboardConfiguration(int32_t client_id, const KeyboardConfiguration &keyboard_configuration) override;
  void SetEditingState(const EditingState &editing_state) override;
  void SetCaretRect(const TRect &caret_rect) override;
  void SetUndoState(bool can_undo, bool can_redo) override;

 protected:
  void HandleKeyboardEvent(const std::shared_ptr<KeyboardEvent> &event) override;
  void OnKeyDown(const std::shared_ptr<KeyboardEvent> &event) override;
  void OnKeyboardStateEvent(const std::shared_ptr<KeyboardStateEvent> &event) override;

 private:
  void RunOnMainThread(dispatch_block_t block);
  TDFTextInputPlugin *textInputPlugin_;
  NSTimer *cursorTimer_;
};

}  // namespace tdfcore

@interface TDFTextInputPlugin : NSObject

- (instancetype)initWithDelegate:(std::weak_ptr<tdfcore::TextInputDelegate>)delegate;

- (void)showTextInputWithCaretRect:(CGRect)caretRect;

- (void)hideTextInput;

- (void)setTextInputClient:(int)client withConfiguration:(const tdfcore::KeyboardConfiguration)configuration;

- (void)setTextInputEditingState:(const tdfcore::EditingState &)editing_state;

- (void)setTextInputFrame:(CGRect)frame;

- (void)clearTextInputClient;

- (void)updateCanUndo:(BOOL)canUndo canRedo:(BOOL)canRedo;

- (UIView<UITextInput> *)textInputView;

@end

@interface DFTextPosition : UITextPosition

@property(nonatomic, readonly) NSUInteger index;

+ (instancetype)positionWithIndex:(NSUInteger)index;
- (instancetype)initWithIndex:(NSUInteger)index;

@end

@interface DFTextRange : UITextRange <NSCopying>

@property(nonatomic, readonly) NSRange range;

+ (instancetype)rangeWithNSRange:(NSRange)range;

@end
