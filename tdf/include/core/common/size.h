//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include "core/common/scalar.h"

namespace tdfcore {

struct TSize {
  TScalar width = 0;
  TScalar height = 0;

  static TSize Make(TScalar w, TScalar h) { return {w, h}; }

  static TSize MakeEmpty() { return {0, 0}; }

  bool IsZero() const { return 0 == width && 0 == height; }

  bool IsEmpty() const { return width <= 0 || height <= 0; }

  friend inline bool operator==(const TSize& a, const TSize& b) { return a.width == b.width && a.height == b.height; }

  friend inline bool operator!=(const TSize& a, const TSize& b) { return !(a == b); }
};

struct ISize {
  int32_t width;
  int32_t height;

  static constexpr ISize Make(int32_t w, int32_t h) { return {w, h}; }

  static constexpr ISize MakeEmpty() { return {0, 0}; }

  void Set(int32_t w, int32_t h) { *this = ISize{w, h}; }

  bool IsZero() const { return 0 == width && 0 == height; }

  bool IsEmpty() const { return width <= 0 || height <= 0; }

  void SetEmpty() { width = height = 0; }

  constexpr int32_t Width() const { return width; }
  constexpr int32_t Height() const { return height; }

  constexpr int64_t Area() const { return width * height; }

  bool Equals(int32_t w, int32_t h) const { return width == w && height == h; }
};

static inline bool operator==(const ISize& a, const ISize& b) { return a.width == b.width && a.height == b.height; }

static inline bool operator!=(const ISize& a, const ISize& b) { return !(a == b); }

}  //  namespace tdfcore
