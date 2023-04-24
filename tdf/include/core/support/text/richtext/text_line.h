//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/graphics/canvas.h"
#include "core/support/text/richtext/line_metrics.h"
#include "core/support/text/richtext/text_run.h"
#include "core/support/text/richtext/text_style.h"

namespace tdfcore {

class TextLine : public Object {
 public:
  /**
   * @brief 构造 函数
   * @param text_runs 当前行包含的所有TextRun
   * @param line_metrics 行测量信息
   * @param width 宽度
   * @param end_with_hard_break 是否以换行符结束
   */
  TextLine(const std::vector<std::shared_ptr<TextRun>>& text_runs, const LineMetrics& line_metrics, TScalar width,
           bool end_with_hard_break)
      : text_runs_(text_runs), line_metrics_(line_metrics), width_(width), end_with_hard_break_(end_with_hard_break) {}

  /**
   * @brief 创建空行，主要用于多个硬换行符匹配多个空行
   * @param line_metrics
   * @param text_position 光标点击到空行时选中文字的位置
   * @return
   */
  static std::shared_ptr<TextLine> MakeEmptyLine(const LineMetrics& line_metrics, int text_position);

  /**
   * @brief 绘制接口
   * @param canvas
   */
  void Paint(Canvas* canvas);

  /**
   * @brief 返回行高
   * @return
   */
  TScalar GetHeight() const { return line_metrics_.GetHeight(); }

  /**
   * @brief 返回行宽
   * @return
   */
  TScalar GetWidth() const { return width_; }

  /**
   * @brief 文字对齐
   * @param align 对齐方式
   * @param width
   */
  void FormatAlign(HorizontalAlign align, TScalar width);

  /**
   * @brief 返回dx位置处的字符位置
   * @param dx
   * @return
   */
  TextPosition GetTextPositionAtCoordinate(TScalar dx);

  /**
   * @brief 给定范围[start, end) 返回所有的bounds信息，如果有换行会返回多个bounds
   * @param start
   * @param end
   * @return
   */
  std::vector<TRect> GetRectsForRange(uint32_t start, uint32_t end);

  /**
   * @brief 返回当前行所有的placeholder信息
   * @return
   */
  std::vector<TRect> GetRectsForPlaceholders();

 private:
  TextLine(const LineMetrics& line_metrics, int text_position);
  void Justify(TScalar width);
  void PaintTextRun(Canvas* canvas, const std::shared_ptr<TextRun>& text_run);
  void PaintDecoration(Canvas* canvas, const std::shared_ptr<TextRun>& text_run);
  void PaintShadows(Canvas* canvas, const std::shared_ptr<TextRun>& text_run);
  std::vector<std::shared_ptr<TextRun>> text_runs_;
  LineMetrics line_metrics_;
  TScalar width_;
  bool end_with_hard_break_;
  int text_position_ = 0;
  bool is_empty_line_ = false;
  TScalar align_shift_ = 0;
  FRIEND_OF_TDF_ALLOC
};

}  // namespace tdfcore