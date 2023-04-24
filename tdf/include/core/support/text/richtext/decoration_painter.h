//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/graphics/canvas.h"
#include "core/support/text/richtext/text_run.h"
#include "core/support/text/richtext/text_style.h"

namespace tdfcore {

class DecorationPainter {
 public:
  void Paint(Canvas* canvas, const std::shared_ptr<TextRun>& run, TScalar baseline);

 private:
  void CalculateThickness(const std::shared_ptr<TextRun>& run);
  void CalculatePosition(const std::shared_ptr<TextRun>& run, TextDecoration decoration, TScalar baseline);
  void GetPaint(const TextStyle& text_style);
  Path CalculateGaps(const std::shared_ptr<TextRun>& run, const TRect& rect, TScalar baseline, TScalar halo);

  TPaint paint;
  TScalar position;
  TScalar thickness;
};

}  // namespace tdfcore