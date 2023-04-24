//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/graphics/canvas.h"
#include "core/support/font/font_manager.h"
#include "core/support/text/richtext/inline_span.h"
#include "core/support/text/richtext/placeholder.h"
#include "core/support/text/richtext/text_run.h"
#include "core/support/text/shaper/text_shaper.h"
#include "core/support/text/unicode/unicode.h"

namespace tdfcore {
class TextLine;
struct ParagraphStyle {
  // 文本方向
  TextDirection direction = TextDirection::kLtr;
  // 水平对齐方式
  HorizontalAlign horizontal_align = HorizontalAlign::kLeft;
  // 垂直对齐方式
  VerticalAlign vertical_align = VerticalAlign::kTop;
  // 最大行数
  size_t max_lines = std::numeric_limits<size_t>::max();
  std::string ellipsis;
  // 默认几倍行高
  TScalar height_multiplier = 1;
  // 默认文本样式，如果TextSpan没有设置TextStyle，则会默认用这个参数
  TextStyle default_text_style;

  bool operator==(const ParagraphStyle& rhs) const {
    return this->direction == rhs.direction && this->horizontal_align == rhs.horizontal_align &&
           this->vertical_align == rhs.vertical_align && this->max_lines == rhs.max_lines &&
           this->ellipsis == rhs.ellipsis && this->height_multiplier == rhs.height_multiplier &&
           default_text_style == rhs.default_text_style;
  }
};

class RichText : public Object, public TextShaper::RunHandler, public std::enable_shared_from_this<RichText> {
 public:
  explicit RichText(const ParagraphStyle& style = ParagraphStyle(),
                    const std::shared_ptr<TFontManager> font_manager = TFontManager::Default(),
                    const std::shared_ptr<TUnicode>& unicode = TUnicode::Make())
      : paragraph_style_(style), font_manager_(font_manager), unicode_(unicode) {}
  /**
   * @brief 设置Span队列数据
   * @param text_span_list
   */
  void SetSpanList(const std::vector<std::shared_ptr<InlineSpan>>& span_list);

  /**
   * @brief 返回Span队列数据
   * @return
   */
  std::vector<std::shared_ptr<InlineSpan>> GetSpanList() const { return span_list_; }

  /**
   * @brief 执行排版
   * @param width 最大限制宽度
   * @param height 最大限制高度，如果用默认值则垂直对齐方式会不生效
   */
  void Layout(TScalar width, TScalar height = std::numeric_limits<TScalar>::infinity());

  /**
   * @brief 获取排版后文字bounds信息，需要在排版之后调用获取的bounds才是正确的
   * @return
   */
  TRect GetMeasuredBounds() const;

  /**
   * @brief 绘制
   * @param canvas
   * @param x 偏移x
   * @param y 偏移y
   */
  void Paint(Canvas* canvas, TScalar x, TScalar y);

  /**
   * @brief 设置段落样式
   * @param style
   */
  void SetParagraphStyle(const ParagraphStyle& style);

  /**
   * @brief 设置文本内容，会替换掉之前的文本
   * @param text
   */
  void SetText(const std::u16string& text);

  /**
   * @brief 获取u16string纯字符数据
   * @return
   */
  std::u16string GetText() const;

  /**
   * @brief 获取坐标(dx, dy)位置处的文字索引位置，为utf16的字符索引
   * @param dx x坐标
   * @param dy y坐标
   * @return
   */
  TextPosition GetTextPositionAtCoordinate(const TPoint& offset);

  /**
   * @brief 获取字符范围[start, end)的所有rect信息
   * @param start
   * @param end
   * @return 多个Rect信息
   */
  std::vector<TRect> GetRectsForRange(uint32_t start, uint32_t end);

  /**
   * @brief 获取所有占位排版元素Rect信息
   * @return
   */
  std::vector<TRect> GetRectsForPlaceholders();

  /**
   * @brief 获取字符位置的单词范围
   * @param offset
   * @return
   */
  TextRange GetWordBoundary(unsigned int offset);

  /**
   * @brief 获取行数
   * @return
   */
  uint32_t GetLineCount() const { return text_lines_.size(); }

  /**
   * @brief 获取第一个应用的TextSpan
   * @return
   */
  TextStyle GetFirstSpanTextStyle() const;

  /**
   * @brief 设置文字裁剪区域，不在rect区域的文字绘制DrawCall会被裁剪掉
   * @param rect
   */
  void SetCullRect(const std::optional<TRect>& rect) { cull_rect_ = rect; }

  /**
   * @brief 获取起始位置光标Rect
   * @param caret_prototype 光标原型
   * @return
   */
  TRect GetStartCaretRect(const TRect& caret_prototype) const;

  bool IsMultiLine() const { return paragraph_style_.max_lines != 1; }

 private:
  // 处理塑形结果
  void BeginLine() override {}
  void RunInfo(const Info&) override {}
  void CommitRunInfo() override {}
  void CommitLine() override {}
  TextShaper::RunHandler::Buffer RunBuffer(const Info& info) override;
  void CommitRunBuffer(const Info&) override;

  void LayoutTextRuns(TScalar width);
  void FormatVerticalAlign(TScalar height);
  void ShapeText();
  void ShaperBlock(size_t start, size_t end);
  void CollectTextSpan();
  void EndLastBlockIfNeed();
  void StartNewBlock(const TextStyle& text_style);
  void PushPlaceholder(const PlaceholderStyle& placeholder);
  void BuildUTF8ToUTF16Mapping();
  int CalculateLineBreak(const std::shared_ptr<TextRun>& run, TScalar remain_width, int& hard_break_count);

  std::shared_ptr<TFont> MatchFont(const TextStyle& text_style);

  ParagraphStyle paragraph_style_;
  std::vector<std::shared_ptr<InlineSpan>> span_list_;
  std::string utf8_str_;
  std::vector<Block> style_blocks_;
  TextStyle default_text_style_;
  std::shared_ptr<TFontManager> font_manager_;
  std::vector<Placeholder> placeholders_;
  TScalar height_multiplier_ = 0;
  TScalar height_ = 0;
  TScalar width_ = 0;
  TScalar vertical_align_shift_ = 0;
  TextRange current_text_ = TextRange::MakeEmpty();
  bool use_half_leading_;
  TScalar baseline_shift_ = 0;
  Vector advance_;
  int64_t unique_run_id_;
  std::shared_ptr<TextRun> current_text_run_;
  TextStyle current_style_;
  bool needs_layout_ = true;
  TSize last_layout_size_;
  std::vector<std::shared_ptr<TextRun>> text_runs_;
  std::vector<std::shared_ptr<TextLine>> text_lines_;
  std::vector<uint32_t> utf16_map_to_utf8_index_;
  std::vector<uint32_t> utf8_map_to_16_index_;
  std::shared_ptr<TUnicode> unicode_;
  std::vector<uint32_t> words_;
  std::optional<TRect> cull_rect_;
};

}  // namespace tdfcore