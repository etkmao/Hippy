//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/text/richtext/placeholder.h"
#include "core/support/text/richtext/text_style.h"
#include "core/support/text/shaper/text_shaper.h"

namespace tdfcore {
class TextRun : public Object {
 public:
  TextRun(const TextShaper::RunHandler::Info& info, size_t first_char, TScalar height_multiplier,
              bool use_half_leading, TScalar baseline_shift, size_t index, TScalar shiftx);

  static std::shared_ptr<TextRun> MakePlaceholder(const TextShaper::RunHandler::Info& info,
                                                      const PlaceholderStyle& style);

  TextRun& operator=(const TextRun&) = delete;
  TextRun& operator=(TextRun&&) = delete;

  TextShaper::RunHandler::Buffer NewRunBuffer();

  TScalar GetPosX(size_t index) const { return positions_[index].x; }
  TScalar GetAdvanceX(size_t index) const { return advances_[index].x; }
  void AddX(size_t index, TScalar shift) { positions_[index].x += shift; }
  TScalar GetPosY(size_t index) const { return positions_[index].y; }
  size_t GetSize() const { return glyph_ids_.size(); }

  void SetWidth(TScalar width) { advance_.x = width; }
  TScalar GetWidth() const { return advance_.x; }
  void SetHeight(TScalar height) { advance_.y = height; }
  void Shift(TScalar shiftX, TScalar shiftY) {
    offset_.x += shiftX;
    offset_.y += shiftY;
  }

  Vector GetAdvance() const {
    return Vector::Make(advance_.x, font_metrics_.descent - font_metrics_.ascent + font_metrics_.leading);
  }

  Vector GetOffset() const { return offset_; }
  TScalar GetAscent() const { return font_metrics_.ascent + baseline_shift_; }
  TScalar GetDescent() const { return font_metrics_.descent + baseline_shift_; }
  TScalar GetLeading() const { return font_metrics_.leading; }
  TScalar GetCorrectAscent() const { return correct_ascent_ + baseline_shift_; }
  TScalar GetCorrectDescent() const { return correct_descent_ + baseline_shift_; }
  TScalar GetCorrectLeading() const { return correct_leading_; }

  const std::shared_ptr<TFont>& GetFont() const { return font_; }
  //  Direction GetTextDirection() const { return direction_; }

  size_t GetIndex() const { return index_; }
  TScalar GetHeightMultiplier() const { return height_multiplier_; }
  bool UseHalfLeading() const { return use_half_leading_; }
  TScalar GetBaselineShift() const { return baseline_shift_; }

  bool IsPlaceholder() const { return is_placeholder_; }

  size_t GetClusterIndex(size_t pos) const { return cluster_index_[pos]; }
  size_t GetGlobalClusterIndex(size_t pos) const { return cluster_start_ + cluster_index_[pos]; }

  TextRange GetTextRange() const { return text_range_; }
  TextRange GetClusterRange() const { return cluster_range_; }

  void CalculateMetrics();

  void SetClusterRange(size_t from, size_t to) { cluster_range_ = TextRange(from, to); }

  void SetTextStyle(const TextStyle& text_style) { text_style_ = text_style; }
  TextStyle GetTextStyle() const { return text_style_; }
  PlaceholderStyle GetPlaceholderStyle() const { return placeholder_style_; }
  TScalar AddLetterSpaces(TScalar space);
  std::shared_ptr<TextRun> Split(int index, int hard_break_count);
  void TrimHardLineBreakAtBegin(int count);
  void TrimHardLineBreakAtEnd(int count);
  std::shared_ptr<TextBlob> MakeTextBlob();
  int GetGlyphPosition(uint32_t cluster_index);

 private:
  TextRun(const TextShaper::RunHandler::Info& info, const PlaceholderStyle& style);
  TextRange text_range_ = TextRange::MakeEmpty();
  std::shared_ptr<TFont> font_;
  std::vector<TPoint> positions_;
  std::vector<GlyphID> glyph_ids_;
  std::vector<TPoint> offsets_;
  std::vector<uint32_t> cluster_index_;
  TFontMetrics font_metrics_;
  TextShaper::Direction direction_;

  TextRange cluster_range_ = TextRange::MakeEmpty();
  int32_t cluster_start_;
  TScalar height_multiplier_;
  bool use_half_leading_;
  TScalar baseline_shift_;
  int64_t index_;
  Vector advance_;
  Vector offset_;
  std::vector<TPoint> advances_;
  TextShaper::RunHandler::Range utf8_range_;
  bool is_placeholder_ = false;
  PlaceholderStyle placeholder_style_;

  TScalar correct_ascent_;
  TScalar correct_descent_;
  TScalar correct_leading_;

  TextStyle text_style_;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore