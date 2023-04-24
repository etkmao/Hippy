//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include <string>
#include "core/common/object.h"
#include "core/support/text/text_base.h"
#include "core/platform/common/keyboard/keyboard.h"

namespace tdfcore {

/// 文字输入模型，并提供了常见操作
class TextInputModel : public Object {
 public:
  TextInputModel();
  ~TextInputModel() override;

  /**
   * @brief 重设text，UTF-16，同时重置selection_和composing_range_
   */
  void SetText(const std::u16string &text);

  /**
   * @brief 设置选择范围（当范围长度为0，可视为光标）
   */
  bool SetSelection(const TextRange &range);

  /**
   * @brief 设置编辑范围
   */
  bool SetComposingRange(const TextRange &range, size_t cursor_offset);

  /**
   * @brief 进入编辑模式
   */
  void BeginComposing();

  /**
   * @brief 用指定文本更新编辑范围，UTF-16
   */
  void UpdateComposingText(const std::u16string &text);

  /**
   * @brief 提交一次编辑，并把光标挪到编辑范围结束位置
   */
  void CommitComposing();

  /**
   * @brief 结束编辑模式
   */
  void EndComposing();

  /**
   * @brief 新增指定文本，UTF-16
   */
  void InsertText(const std::u16string &text);

  /**
   * @brief 删除，删除选中区域或光标后面一个字符
   */
  bool DeleteForward();

  /**
   * @brief 退格，删除选中区域或光标前面一个字符
   */
  bool DeleteBackward();

  /**
   * @brief 往右移动光标
   */
  bool MoveCursorRight();

  /**
   * @brief 往左移动光标
   */
  bool MoveCursorLeft();

  /**
   * @brief 往右移动光标并操作右方字符（选中或取消选中）
   */
  bool MoveCursorRightAndSelect();

  /**
   * @brief 往左移动光标并操作左方字符（选中或取消选中）
   */
  bool MoveCursorLeftAndSelect();

  /**
   * @brief 移动光标到开始位置
   */
  bool MoveCursorToBeginning();

  /**
   * @brief 移动光标到结束位置
   */
  bool MoveCursorToEnd();

  /**
   * @brief 选中所有文本字符
   */
  bool SelectAllText();

  /**
   * @brief 获取文本UTF-16
   */
  std::u16string GetText() const { return text_; }

  /**
   * @brief 返回整个文本range
   */
  TextRange text_range() const { return {0, static_cast<int64_t>(text_.length())}; }

  /**
   * @brief 返回选中的range
   */
  TextRange selection() const { return selection_; }

  /**
   * @brief 返回编辑的range
   */
  TextRange composing_range() const { return composing_range_; }

  /**
   * @brief 是否激活了编辑模式
   */
  bool composing() const { return composing_; }

  /**
   * @brief 删除选中区域
   */
  bool DeleteSelected();

  /***
   * @brief 返回EditingState数据结构
   * @param text_affinity
   * @return
   */
  EditingState GetEditingState(EditingState::TextAffinity text_affinity = EditingState::TextAffinity::kUpstream) const;

  /***
   * @brief 把EdigintState信息更新到TextInputModel
   */
  void UpdateFromEditingState(const EditingState& editing_state);

 private:
  /**
   * @brief 返回可编辑区域
   */
  TextRange editable_range() const { return composing_ ? composing_range_ : text_range(); }

  // 记录部分选中text位置数据
  TextPosition offset_position_;
  std::u16string text_;
  TextRange selection_ = TextRange(0, 0);
  TextRange composing_range_ = TextRange(0, 0);
  bool composing_ = false;
};
}  // namespace tdfcore
