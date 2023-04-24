//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <any>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include "core/common/object.h"
#include "core/common/rect.h"
#include "core/common/serialization.h"
#include "core/platform/common/keyboard/keyboard_constants.h"
#include "core/support/text/text_base.h"

namespace tdfcore {
/**
 * @brief 键盘行为枚举
 */
enum class KeyboardAction : uint8_t {
  kNone,
  kUnspecified,
  kGo,
  kSearch,
  kSend,
  kNext,
  kPrevious,
  kContinueAction,
  kJoin,
  kRoute,
  kEmergencyCall,
  kDone,
  kNewLine,
  kDelete,
  kCopy,
  kCut,
  kPaste,
  kSelectAll,
  kUndo,
  kRedo,
};

/**
 * @brief 智能替换破折号，例如连续输入三个'-' 会自动替换成一个'——'
 */
enum SmartDashesType { kDashesNone, kDashesDisabled, kDashesEnabled };

/**
 * @brief 键盘亮度
 */
enum Brightness {
  kDark,
  kLight,
};

/**
 * @brief 智能替换引号，根据文字情况智能替换为左引号或者右引号
 */
enum SmartQuotesType { kQuotesNone, kQuotesDisabled, kQuotesEnabled };

/**
 * @brief 大写类型
 */
enum TextCapitalization {
  kWords,
  kSentences,
  kCharacters,
  kTextCapitalizationNone,
};

/**
 * @brief 游标拖动状态
 */
enum CursorPanState {
  start,
  update,
  end,
};

/**
 * @brief 键盘输入类型
 */
struct KeyboardInputType {
  enum InputType {
    kText,
    kMultiLine,
    kNumber,
    kPhone,
    kDateTime,
    kEmailAddress,
    kUrl,
    kPassword,
  };

  /**
   * @brief 获取Text输入类型
   * @return Text输入类型
   */
  static KeyboardInputType Text();

  /**
   * @brief 获取多行输入类型
   * @return 多行输入类型
   */
  static KeyboardInputType MultiLine();

  /**
   * @brief 获取数字输入类型
   * @return 数字输入类型
   */
  static KeyboardInputType Number();

  /**
   * @brief 获取电话号码输入类型
   * @return 电话号码输入类型
   */
  static KeyboardInputType Phone();

  /**
   * @brief 获取日期输入类型
   * @return 日期输入类型
   */
  static KeyboardInputType DateTime();

  /**
   * @brief 电子邮件输入类型
   * @return 获取电子邮件输入类型
   */
  static KeyboardInputType EmailAddress();

  /**
   * @brief 获取url输入类型
   * @return 输入类型
   */
  static KeyboardInputType Url();

  /**
   * @brief 获取密码输入类型
   * @return 密码输入类型
   */
  static KeyboardInputType Password();

  /**
   * @brief 获取数字输入类型
   * @param is_signed 是否为有符号数字
   * @param is_decimal 是否为十进制
   * @return KeyboardInputType
   */
  static KeyboardInputType Number(bool is_signed, bool is_decimal);

  KeyboardInputType() = default;
  explicit KeyboardInputType(InputType keyboardActionInputType);
  KeyboardInputType(InputType keyboardActionInputType, bool is_signed, bool is_decimal);

  InputType type_index;
  bool is_signed;
  bool is_decimal;
};

/**
 * @brief 键盘配置类
 */
class KeyboardConfiguration {
 public:
  KeyboardConfiguration(KeyboardInputType input_type, bool obscure_text, bool auto_correct,
                        SmartDashesType smart_dashes_type, SmartQuotesType smart_quotes_type, bool enable_suggestions,
                        std::optional<std::string> action_label, KeyboardAction keyboard_action,
                        TextCapitalization text_capitalization, Brightness keyboard_appearance);

  KeyboardInputType input_type_;
  bool obscure_text_;
  bool auto_correct_;
  SmartDashesType smart_dashes_type_;
  SmartQuotesType smart_quotes_type_;
  bool enable_suggestions_;
  const std::optional<std::string> action_label_;
  KeyboardAction keyboard_action_;
  TextCapitalization text_capitalization_;
  Brightness keyboard_appearance_;
};

/**
 * @brief 键盘文本编辑状态
 */
class EditingState {
 public:
  /**
   * @brief 当文本发生换行时，光标是处于上一行末，还是处于下一行首
   */
  enum TextAffinity : uint8_t { kUpstream, kDownstream };

  EditingState() = default;

  EditingState(std::u16string text, int64_t selection_base, int64_t selection_extent, TextAffinity selection_affinity,
               bool selection_is_directional, int64_t composing_base, int64_t composing_extent);

 public:
  std::u16string text_;
  int64_t selection_base_;
  int64_t selection_extent_;
  TextAffinity selection_affinity_;
  bool selection_is_directional_;
  int64_t composing_base_;
  int64_t composing_extent_;

  /**
   * @brief 序列化与反序列化
   */
  TDF_SERIALIZATION(text_, selection_base_, selection_extent_,
                    TDF_SERIALIZABLE_ENUM(selection_affinity_, TextAffinity, uint8_t), selection_is_directional_,
                    composing_base_, composing_extent_);
};

class EventCenter;
class KeyboardEvent;
class KeyboardStateEvent;
class TextInputDelegate {
 public:
  /***
   * @brief 处理在文本提交之前输入法输入的文字
   * @param text
   */
  virtual void SetComposingText(const std::u16string& text) = 0;

  /**
   * @brief 提交输入法未提交的文字
   */
  virtual void CommitComposing() = 0;

  /***
   * @brief 插入文字
   * @param text
   */
  virtual void InsertText(const std::u16string& text, const TextRange& range = TextRange::MakeEmpty()) = 0;

  /**
   * @brief 向后删除一个字符
   */
  virtual void DeleteBackward() = 0;

  /***
   * @brief 处理键盘的特殊动作，例如全选、粘贴等会涉及EditingState变更的动作
   * @param action
   */
  virtual void OnKeyboardAction(KeyboardAction action) = 0;

  /***
   * @brief 键盘基础事件发送
   * @param event
   */
  virtual void OnKeyboardEvent(const std::shared_ptr<KeyboardEvent>& event) = 0;

  /**
   * @brief 键盘状态事件
   */
  virtual void OnKeyboardStateEvent(const std::shared_ptr<KeyboardStateEvent>& event) = 0;
};

class TextInputModel;
/**
 * @brief 抽象键盘能力类，外部调用者要保证在Platform调用
 * */
class Keyboard : public Object, public TextInputDelegate, public std::enable_shared_from_this<Keyboard> {
 public:
  Keyboard();

  /**
   * @brief 弹起键盘
   */
  virtual void Show() = 0;

  /**
   * @brief 隐藏键盘
   */
  virtual void Hide() = 0;

  /**
   * @brief 是否正在展示键盘
   */
  virtual bool IsShowing() { return is_showing_; }

  /**
   * @brief 设置键盘配置和当前键盘使用者的id，内核用client id来记录当前操作键盘的组件标识
   * @param client_id 记录当前键盘使用者的标识
   * @param keyboard_configuration 键盘配置
   */
  virtual void SetKeyboardConfiguration(int32_t client_id, const KeyboardConfiguration& keyboard_configuration) = 0;

  /**
   * @brief 设置编辑信息
   * @param editing_state 编辑信息e
   */
  virtual void SetEditingState(const EditingState& editing_state) = 0;

  /**
   * @brief 设置光标基于窗口的rect
   * @param caret_rect rect
   */
  virtual void SetCaretRect(const TRect& caret_rect) = 0;

  /**
   * @brief 更新Undo的状态
   * 仅在Keyboard有焦点的时候生效
   * @param can_undo 是否可以撤销
   * @param can_redo 是否可以重做
   */
  virtual void SetUndoState(bool can_undo, bool can_redo){};

  /**
   * @brief 获取光标基于窗口的rect
   */
  constexpr TRect GetCaretRect() const { return caret_rect_; }

  /***
   * @brief 设置EventCenter
   * @param event_center
   */
  void SetEventCenter(const std::shared_ptr<EventCenter>& event_center);

  /// TextInputDelegate
  void SetComposingText(const std::u16string& text) override;
  void CommitComposing() override;
  void InsertText(const std::u16string& text, const TextRange& range = TextRange::MakeEmpty()) override;
  void DeleteBackward() override;
  void OnKeyboardAction(KeyboardAction action) override;
  void OnKeyboardEvent(const std::shared_ptr<KeyboardEvent>& event) override;
  void OnKeyboardStateEvent(const std::shared_ptr<KeyboardStateEvent>& event) override;

 protected:
  /// TextInput处理完之后更新EditingState
  void UpdateEditingState();
  void DeleteForward();
  virtual void HandleKeyboardEvent(const std::shared_ptr<KeyboardEvent>& event);
  virtual void OnKeyDown(const std::shared_ptr<KeyboardEvent>& event);

  EditingState::TextAffinity text_affinity_;
  std::shared_ptr<TextInputModel> text_input_model_;
  std::shared_ptr<EventCenter> event_center_;
  int32_t client_id_ = -1;
  bool is_showing_ = false;
  TRect caret_rect_;
  uint64_t keyboard_event_listener_ = 0;
  ModifierFlag ctrl_modifier_flag_ = ModifierFlag::kControl;
};
}  // namespace tdfcore
