//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/text_object.h"
#include "tdfui/view/view.h"

namespace tdfcore {

class TextView : public View {
  TDF_REFF_CLASS_META(TextView)

 public:
  void Init() override;

  /**
   * @brief 增加一个Span元素，没有传index或者index=-1则会增加Span到末尾，否则会插入到index之前的位置
   * @param span
   * @param index 插入位置索引
   */
  void AddSpan(const std::shared_ptr<InlineSpan>& span, int index = -1);

  /**
   * @brief 删除一个Span元素，没有传index或者index=-1则删除末尾元素，否则会删除index处的Span
   * @param index 插入位置索引
   */
  void RemoveSpan(int index = -1);

  /**
   * @brief 设置Span列表
   * @param span_list
   */
  void SetSpanList(const std::vector<std::shared_ptr<InlineSpan>>& span_list);

  /**
   * @brief 获取Span列表
   * @return
   */
  std::vector<std::shared_ptr<InlineSpan>> GetSpanList() const;

  /**
   * @brief 获取占位元素排版后的rect信息，此函数调用需要在TextView::Layout完成之后获取的值才是正确的
   * @return
   */
  std::vector<TRect> GetRectsForPlaceholders();
  /**
   * @brief 设置TextView文字内容，会替换掉之前的TextSpan内容
   * @param text 文字
   */
  void SetText(const std::u16string& text);

  /**
   * @brief 获取TextView文字字符内容，如果有多个TextSpan则会把所有字符拼接到一起
   * @return
   */
  std::u16string GetText() const;

  /**
   * @brief 设置默认文字风格，如果TextSpan没有设置TextStyle，则会统一用这个TextStyle
   * @param text_style 文字风格
   */
  void SetTextStyle(const TextStyle& text_style);

  /**
   * @brief 获取当前默认TextStyle
   * @return
   */
  TextStyle GetTextStyle() const;

  /**
   * @brief 设置最大行数
   * @param max_lines 最大行数
   */
  void SetMaxLines(size_t max_lines);

  /**
   * @brief 获取最大行数
   * @return 最大行数
   */
  size_t GetMaxLines() const { return paragraph_style_.max_lines; }

  /**
   * @brief 设置水平对齐方式
   * @param align 水平对齐方式
   */
  void SetHorizontalAlign(HorizontalAlign align);

  /**
   * @brief 获取水平对齐方式
   * @return 水平对齐方式
   */
  HorizontalAlign GetHorizontalAlign() const { return paragraph_style_.horizontal_align; }

  /**
   * @brief 设置垂直对齐方式
   * @param align
   */
  void SetVerticalAlign(VerticalAlign align);

  /**
   * @brief 获取垂直对齐方式
   * @return
   */
  VerticalAlign GetVerticalAlign() const { return paragraph_style_.vertical_align; }

  /**
   * @brief 设置文本方向
   * @param text_direction 文本方向
   */
  void SetTextDirection(TextDirection text_direction);

  /**
   * @brief 获取文本方向
   * @return 文本方向
   */
  TextDirection GetTextDirection() const { return paragraph_style_.direction; }

  /**
   * @brief 设置行高
   * @param height 行高
   */
  void SetHeightMultiplier(TScalar height);

  /**
   * @brief 获取行高
   * @return 行高
   */
  TScalar GetHeightMultiplier() const { return paragraph_style_.height_multiplier; }

  /**
   * @brief 测量文本大小
   * @param width 最大宽度
   * @return 测量后的文本大小
   */
  TSize MeasureText(uint64_t width);

  /**
   * @brief 设置裁剪区域，如果TextView文字数量太大，很多超出显示区域，
   * 可以通过设置此参数来裁掉显示区域之外的文字DrawCall，这样性能有很大的提升
   * @param rect
   */
  void SetCullRect(const std::optional<TRect>& rect);

 protected:
  explicit TextView(const std::shared_ptr<ViewContext>& view_context = ViewContext::GetCurrent());
  void Layout() override;

  int32_t GetTotalRenderObjectCount() override;

 private:
  enum RenderObjectIndex { kText = View::kRenderObjectCount, kRenderObjectCount };
  ParagraphStyle paragraph_style_;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::TextView, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::TextView)
