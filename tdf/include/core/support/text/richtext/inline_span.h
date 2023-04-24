//
// Copyright © 2020 com.tencent. All rights reserved.
//

#pragma once

#include "core/support/text/richtext/placeholder.h"
#include "core/support/text/richtext/text_style.h"

namespace tdfcore {
/**
 * @brief Span类型
 */
enum class InlineSpanType { kTextSpan, kPlaceholderSpan };

/**
 * @brief Span元素基类
 * 可以派生出不同类型的Span元素
 */
class InlineSpan : public Object {
 public:
  virtual ~InlineSpan() = default;

  virtual InlineSpanType GetSpanClassType() const = 0;
};

/**
 * @brief 文字元素
 * 管理文字基本信息，包括文字字符以及文字样式
 */
class TextSpan : public InlineSpan {
 public:
  /**
   * @brief 返回Span类型
   * @return TextSpan
   */
  InlineSpanType GetSpanClassType() const override { return InlineSpanType::kTextSpan; }

  /**
   * @brief 获取当前元素string对象
   * @return text字符串
   */
  constexpr const std::u16string &GetText() const { return text_; }
  /**
   * @brief 设置当前元素字符串
   * @param string 需要设置的字符串
   */
  void SetText(const std::u16string &text) { text_ = text; }
  /**
   * @brief 获取文字样式
   * @return 文字样式
   */
  constexpr const std::optional<TextStyle> &GetTextStyle() const { return text_style_; }
  /**
   * @brief 设置文字样式
   * @param text_style 文字样式
   */
  void SetTextStyle(const TextStyle &text_style) { text_style_ = text_style; }

  /**
   * @brief 在字符的index位置处进行拆分，拆成两个TextSpan，返回另一个TextSpan
   * @param index
   * @return
   */
  std::shared_ptr<TextSpan> Split(int index);

  /**
   * @brief 复制出一个TextSpan
   * @return
   */
  std::shared_ptr<TextSpan> Clone();

 protected:
  /**
   * @brief 构造函数
   * @param text
   */
  explicit TextSpan(std::u16string text = u"") : InlineSpan(), text_(std::move(text)) {}

 private:
  std::u16string text_;
  std::optional<TextStyle> text_style_;

  FRIEND_OF_TDF_ALLOC;
};

class PlaceholderSpan : public InlineSpan {
 public:
  InlineSpanType GetSpanClassType() const override { return InlineSpanType::kPlaceholderSpan; }

  /**
   * @brief 获取占位样式
   * @return
   */
  PlaceholderStyle GetPlaceholderStyle() const { return placeholder_style_; }

  /**
   * @brief 设置占位样式
   * @param style
   */
  void SetPlaceholderStyle(const PlaceholderStyle &style) { placeholder_style_ = style; }

 protected:
  /**
   * @brief 构造函数
   * @param style 占位样式
   */
  explicit PlaceholderSpan(const PlaceholderStyle &style) : InlineSpan(), placeholder_style_(style) {}

 private:
  PlaceholderStyle placeholder_style_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
