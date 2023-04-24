//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/text/richtext/text_style.h"

namespace tdfcore {

struct PlaceholderStyle {
  PlaceholderStyle() = default;
  PlaceholderStyle(TScalar width, TScalar height, TextBaseline baseline, TScalar offset)
      : width(width), height(height), baseline(baseline), baseline_offset(offset) {}
  // TODO(lukiyang): 增加对齐方式
  TScalar width = 0;
  TScalar height = 0;
  TextBaseline baseline = TextBaseline::kAlphabetic;
  TScalar baseline_offset = 0;
};

struct Placeholder {
  Placeholder() = default;
  Placeholder(size_t start, size_t end, const PlaceholderStyle& style, const TextStyle& text_style,
              TextRange blocks_before, TextRange text_before)
      : utf8_range(start, end),
        style(style),
        text_style(text_style),
        blocks_before(blocks_before),
        text_before(text_before) {}

  TextRange utf8_range = TextRange::MakeEmpty();
  PlaceholderStyle style;
  TextStyle text_style;
  TextRange blocks_before = TextRange::MakeEmpty();
  TextRange text_before = TextRange::MakeEmpty();
};

}  // namespace tdfcore