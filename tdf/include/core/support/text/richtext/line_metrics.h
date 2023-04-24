//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include "core/common/scalar.h"

namespace tdfcore {

class TFont;
class TextRun;
class LineMetrics {
 public:
  LineMetrics() = default;
  explicit LineMetrics(bool force_strut) : force_strut_(force_strut) {}
  LineMetrics(TScalar ascent, TScalar descent, TScalar leading)
      : ascent_(ascent), descent_(descent), leading_(leading) {}

  LineMetrics(const std::shared_ptr<TFont>& font, bool force_strut);
  void Add(const std::shared_ptr<TextRun>& run);
  void Add(const LineMetrics& other);
  TScalar GetDelta() const { return GetHeight() - GetIdeographicBaseline(); }
  TScalar GetHeight() const { return descent_ - ascent_ + leading_; }
  void Update(TScalar a, TScalar d, TScalar l);

  TScalar GetAlphabeticBaseline() const { return leading_ / 2 - ascent_; }
  TScalar GetIdeographicBaseline() const { return descent_ - ascent_ + leading_; }
  TScalar GetDeltaBaselines() const { return leading_ / 2 + descent_; }
  TScalar GetBaseline() const { return leading_ / 2 - ascent_; }
  TScalar GetAscent() const { return ascent_; }
  TScalar GetDescent() const { return descent_; }
  TScalar GetLeading() const { return leading_; }
  void SetForceStrut(bool value) { force_strut_ = value; }
  bool GetForceStrut() const { return force_strut_; }
  void Reset();

 private:
  TScalar ascent_ = kTScalarMax;
  TScalar descent_ = -kTScalarMax;
  TScalar leading_ = 0;
  bool force_strut_ = false;
};
}  // namespace tdfcore