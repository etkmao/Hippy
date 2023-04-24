//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <list>
#include <memory>
#include <string>
#include <utility>
#include "core/common/diagnosticable.h"
#include "core/platform/common/keyboard/keyboard.h"
#include "core/support/text/text_base.h"
#include "core/tdfi/event.h"

namespace tdfcore {

class Shell;

enum class FloatingCursorDragState {
  /// A user has just activated a floating cursor.
  Start,

  /// A user is dragging a floating cursor.
  Update,

  /// A user has lifted their finger off the screen after using a floating
  /// cursor.
  End,
};

enum class TextValueType { kTextEditingValue };
class ClipboardStatusNotifier;

class TextEditingValue : public Diagnosticable {
 public:
  explicit TextEditingValue(std::u16string text = u"", TextSelection selection = TextSelection(),
                            TextRange range = TextRange::MakeEmpty());
  TextValueType GetTextValueType() const { return TextValueType::kTextEditingValue; }
  bool operator==(const TextEditingValue &other) const;
  bool operator!=(const TextEditingValue &other) const;
  std::shared_ptr<Diagnostics> ToDiagnostics() const override;

  constexpr const std::u16string &GetText() const { return text_; }

  constexpr const TextSelection &GetSelection() const { return text_selection_; }

  constexpr const TextRange &GetComposing() const { return composing_; }

  void SetSelection(const TextSelection &text_selection) { text_selection_ = text_selection; }

  void SetText(const std::u16string &text) { text_ = text; }

  static TextEditingValue Trim(const TextEditingValue &value, size_t target_length, bool adjust_selection = true);

 private:
  std::u16string text_;
  TextSelection text_selection_;
  TextRange composing_ = TextRange(-1, -1);
};

struct RawFloatingCursorPoint {
  TPoint offset;
  FloatingCursorDragState state;
};

class TextInputConnection;
class Timer;
/**
 * 维护TextInputConnection，工作与Platform TaskRunner
 */
class TextInput : public Object, public std::enable_shared_from_this<TextInput> {
 public:
  ~TextInput() override = default;

  /**
   * @brief 显示键盘
   */
  void Show(TRect caret_rect, const std::shared_ptr<TextInputConnection> &conn);

  /**
   * @brief 更新光标位置
   */
  void UpdateCaretRect(TRect caret_rect, const std::shared_ptr<TextInputConnection> &conn);

  /**
   * @brief 隐藏键盘
   */
  void Hide(const std::shared_ptr<TextInputConnection> &conn);

  /**
   * @brief 关闭键盘
   * @detail 如果调用了关闭键盘，接着又Attach了下一个TextInputConnection，则不会隐藏键盘
   */
  void Close(const std::shared_ptr<TextInputConnection> &conn);

  /**
   * @brief 设置键盘文本编辑状态
   */
  void SetEditingState(const EditingState &editing_state, const std::shared_ptr<TextInputConnection> &conn);

  /**
   * @brief 绑定text input connection
   */
  void Attach(const std::shared_ptr<TextInputConnection> &connection, const KeyboardConfiguration &configuration);
  void Detach(const std::shared_ptr<TextInputConnection> &connection);

  bool IsAttached(const std::shared_ptr<TextInputConnection> &conn);

  /**
   * @brief 获取新的TextInputConnection
   */
  std::shared_ptr<TextInputConnection> MakeTextInputConnection() const;

  /**
   * @brief 获取当前的text input connection
   */
  std::shared_ptr<TextInputConnection> GetCurrentConnection() const { return current_connection_; }

 protected:
  explicit TextInput(const std::weak_ptr<Shell> &shell);

 private:
  void AddKeyboardEventListener();
  void RemoveKeyboardEventListener();

  std::shared_ptr<TextInputConnection> current_connection_;
  uint64_t keyboard_action_listener_id_ = 0;
  uint64_t keyboard_editing_state_listener_id_ = 0;
  uint64_t keyboard_event_listener_id_ = 0;
  std::weak_ptr<Shell> shell_;

  FRIEND_OF_TDF_ALLOC;
};

class TextInputConnection : public Object, public std::enable_shared_from_this<TextInputConnection> {
 public:
  using EditingStateListener = std::function<void(const EditingState &)>;
  using KeyboardActionListener = std::function<void(KeyboardAction)>;
  using KeyboardEventListener = std::function<void(const std::shared_ptr<KeyboardEvent> &)>;

  ~TextInputConnection() override = default;
  explicit TextInputConnection(const std::weak_ptr<Shell> &shell);
  void Show(TRect caret_rect);
  void UpdateCaretRect(TRect caret_rect);
  void SetEditingState(const EditingState &editing_state);
  void SetEditableSizeAndTransform();
  void SetStyle();
  void Close();
  void Hide();
  void ConnectionClosedReceived();
  int GetClientId() const { return client_id_; }
  void NotifyEditingStateListener(const EditingState &state) const;
  void NotifyKeyboardActionListener(KeyboardAction action) const;
  void NotifyKeyboardEventListener(const std::shared_ptr<KeyboardEvent> &) const;
  void SetTextInput(const std::weak_ptr<TextInput> &text_input) { text_input_ = text_input; }

  void SetEditingStateListener(const EditingStateListener &callback) { editing_state_listener_ = callback; }

  void SetKeyboardActionListener(const KeyboardActionListener &callback) { keyboard_action_listener_ = callback; }

  void SetKeyboardEventListener(const KeyboardEventListener &callback) { keyboard_event_listener_ = callback; }

 private:
  int client_id_;
  EditingStateListener editing_state_listener_;
  KeyboardActionListener keyboard_action_listener_;
  KeyboardEventListener keyboard_event_listener_;
  std::weak_ptr<Shell> shell_;
  std::weak_ptr<TextInput> text_input_;
};

class TextInputFormatter {
 public:
  virtual TextEditingValue FormatEditUpdate(TextEditingValue old_value, TextEditingValue new_value) = 0;
};

class TextSelectionDelegate {
 public:
  virtual const TextEditingValue &GetTextEditingValue() = 0;

  /**
   * 非输入法更新文本编辑值时会执行的方法，例如用户拖拽改变选区、复制、粘贴等
   * @param value 新的编辑值
   */
  virtual void UserUpdateTextEditingValue(const TextEditingValue &value) = 0;

  /**
   * 在视图中显示指定位置的文字。
   * 例如`position=10`超出视图范围，调用此接口后可会执行滚动逻辑确保目标文字在可视范围中
   */
  virtual void BringIntoView(TextPosition position) = 0;

  /**
   * 是否允许剪切操作
   */
  virtual bool IsCutEnabled() = 0;

  virtual bool IsCopyEnabled() = 0;
  virtual bool IsPasteEnabled() = 0;
  virtual bool IsSelectAllEnabled() = 0;

  /**
   * 处理剪切动作
   */
  virtual void HandleCut() = 0;

  virtual void HandleCopy() = 0;
  virtual void HandlePaste() = 0;
  virtual void HandleSelectAll() = 0;
};

}  // namespace tdfcore
