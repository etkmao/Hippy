//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include <cmath>
#include <string>
#include "core/common/scalar.h"

namespace tdfcore {

struct TPoint;
using Vector = TPoint;

struct TPoint {
  TScalar x = 0;
  TScalar y = 0;

  static constexpr TPoint Make(TScalar x, TScalar y) { return {x, y}; }

  std::string ToString() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }

  void Set(TScalar val_x, TScalar val_y) {
    this->x = val_x;
    this->y = val_y;
  }

  TScalar Length() const { return std::sqrt(x * x + y * y); }

  constexpr bool IsZero() const { return (0 == x) & (0 == y); }

  TScalar DistanceToOrigin() const { return Length(); }

  bool IsFinite() const { return !std::isnan(x * y); }

  void Offset(TScalar dx, TScalar dy) {
    x += dx;
    y += dy;
  }

  TPoint operator-() const { return {-x, -y}; }

  void operator-=(const TPoint &p) {
    x -= p.x;
    y -= p.y;
  }

  TPoint operator*(TScalar scale) const { return {x * scale, y * scale}; }

  TPoint &operator*=(TScalar scale) {
    x *= scale;
    y *= scale;
    return *this;
  }

  friend bool operator==(const TPoint &a, const TPoint &b) { return a.x == b.x && a.y == b.y; }

  friend bool operator!=(const TPoint &a, const TPoint &b) { return a.x != b.x || a.y != b.y; }

  friend Vector operator-(const TPoint &a, const TPoint &b) { return {a.x - b.x, a.y - b.y}; }

  friend TPoint operator+(const TPoint &a, const Vector &b) { return {a.x + b.x, a.y + b.y}; }

  void operator+=(const Vector &v) {
    x += v.x;
    y += v.y;
  }
};

}  // namespace tdfcore