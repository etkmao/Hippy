//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <optional>
#include "core/engine/graphics/paint.h"
#include "core/support/font/font_style.h"
#include "core/support/font/typeface.h"
#include "core/support/text/text_base.h"

namespace tdfcore {

// 各平台默认字体
#if TDF_PLATFORM_ANDROID
#  define kDefaultFontFamily "Droid Sans"
#elif TDF_PLATFORM_IOS || TDF_PLATFORM_MACOS
#  define kDefaultFontFamily "PingFang SC"
#elif TDF_PLATFORM_WIN
#  define kDefaultFontFamily "Microsoft YaHei"
#else
#  define kDefaultFontFamily "sans-serif"
#endif

enum TextDecoration {
  kNoDecoration = 0x0,
  kUnderline = 0x1,
  kOverline = 0x2,
  kLineThrough = 0x4,
};
constexpr TextDecoration AllTextDecorations[] = {
    kNoDecoration,
    kUnderline,
    kOverline,
    kLineThrough,
};
enum TextDecorationMode { kGaps, kThrough };

enum class TextDecorationStyle {
  kSolid,   // 一条实线
  kDouble,  // 两条实线
  kDotted,  // 原点虚线
  kDashed,  // 破折号虚线
  kWavy     // 波浪线
};
struct Decoration {
  uint32_t type;
  TextDecorationMode mode;
  Color color;
  TextDecorationStyle style;
  TScalar thickness_multiplier;

  bool operator==(const Decoration& other) const {
    return this->type == other.type && this->mode == other.mode && this->color == other.color &&
           this->style == other.style && this->thickness_multiplier == other.thickness_multiplier;
  }
  bool operator!=(const Decoration& other) const { return !(*this == other); }
};

struct TextShadow {
  Color color = Color::Black();
  TPoint offset;
  double blur_sigma = 0.0;
  TextShadow() = default;
  TextShadow(Color color, TPoint offset, double blur_sigma) : color(color), offset(offset), blur_sigma(blur_sigma) {}

  bool operator==(const TextShadow& rhs) const {
    return color == rhs.color && offset == rhs.offset && blur_sigma == rhs.blur_sigma;
  }

  bool HasShadow() const {
    if (!offset.IsZero()) return true;
    if (blur_sigma != 0.0) return true;
    return false;
  }
};

struct FontFeature {
  FontFeature(const std::string& feature_name, int feature_value) : name(feature_name), value(feature_value) {}
  bool operator==(const FontFeature& that) const { return name == that.name && value == that.value; }
  std::string name;
  int value;
};

struct TextStyle {
  TextStyle() = default;
  bool operator==(const TextStyle& rhs) const;
  // 字体名称
  std::string font_family = std::string(kDefaultFontFamily);
  // 字体大小
  TScalar font_size = 14.0;
  // 几倍行高
  TScalar height = 1.0;
  // 是否用几倍行高替换固有属性行高
  bool height_override = false;
  // baseline偏移
  TScalar baseline_shift = 0.0f;
  // 是否考虑上下leading间距
  bool half_leading = false;
  // 语言
  std::string locale = {};
  // 字间距
  TScalar letter_spacing = 0.0;
  // 单词间距
  TScalar word_spacing = 0.0;
  // 是否加粗
  bool bold = false;
  // 是否斜体
  bool italic = false;

  // baseline对齐方式
  TextBaseline text_baseline_ = TextBaseline::kAlphabetic;
  // 修饰信息
  Decoration decoration = {TextDecoration::kNoDecoration, TextDecorationMode::kThrough, Color::Transparent(),
                            TextDecorationStyle::kSolid, 1.0f};

  // 文字颜色
  Color color = Color::Black();
  // 背景颜色
  std::optional<TPaint> background;
  // 前景颜色
  std::optional<TPaint> foreground;

  // 文字阴影
  std::vector<TextShadow> text_shadows;
  // 文字特征
  std::vector<FontFeature> font_features;
  // 字体Typeface
  std::shared_ptr<TTypeface> typeface;
};

struct Block {
  Block() = default;
  Block(size_t start, size_t end, const TextStyle& style) : range(start, end), style(style) {}
  Block(TextRange text_range, const TextStyle& style) : range(text_range), style(style) {}

  void Add(TextRange tail) { range = TextRange(range.GetStart(), range.GetStart() + range.GetSize() + tail.GetSize()); }

  TextRange range = TextRange::MakeEmpty();
  TextStyle style;
};

}  // namespace tdfcore
