//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/engine/render/render_object.h"
#include "core/support/text/richtext/rich_text.h"

namespace tdfcore {

class TextObject : public RenderObject {
  TDF_REFF_CLASS_META(TextObject)

 public:
  void SetText(const std::u16string& text);

  void AddSpan(const std::shared_ptr<InlineSpan>& span, int index = -1);

  void RemoveSpan(int index = -1);

  void SetSpanList(const std::vector<std::shared_ptr<InlineSpan>>& span_list);

  void SetTextStyle(const TextStyle& text_style);

  void SetMaxLines(size_t max_lines);

  void SetHorizontalAlign(HorizontalAlign align);

  void SetVerticalAlign(VerticalAlign align);

  void SetTextDirection(TextDirection text_direction);

  void SetHeightMultiplier(TScalar height);

  virtual void Layout(uint64_t width, uint64_t height = INT64_MAX);

  TRect MeasureBounds(uint64_t width);

  TRect GetBounds() const;

  std::vector<TRect> GetRectsForPlaceholders() const;

  std::vector<std::shared_ptr<InlineSpan>> GetSpanList() const;

  std::u16string GetText() const;

  void SetCullRect(const std::optional<TRect>& rect);

 protected:
  explicit TextObject(const ParagraphStyle& style = ParagraphStyle(),
                      const std::shared_ptr<TFontManager>& font_manager = TFontManager::Default());
  void OnPaint(PaintingContext& context, const TPoint& offset) override;
  void LayoutText(uint64_t width, uint64_t height);
  std::shared_ptr<RichText> rich_text_;
  ParagraphStyle paragraph_style_;
  bool needs_layout_ = true;
  ISize last_layout_size_ = ISize::MakeEmpty();
  uint64_t last_layout_width_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::TextObject, bases<tdfcore::RenderObject>)
TDF_REFL_END(tdfcore::TextObject)