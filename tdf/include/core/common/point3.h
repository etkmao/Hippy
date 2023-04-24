//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include <cmath>
#include "core/common/scalar.h"

namespace tdfcore {
struct TPoint3 {
  TScalar x = 0;
  TScalar y = 0;
  TScalar z = 0;

  static constexpr TPoint3 Make(TScalar x, TScalar y, TScalar z) { return {x, y, z}; }

  void Set(TScalar val_x, TScalar val_y, TScalar val_z) {
    this->x = val_x;
    this->y = val_y;
    this->z = val_z;
  }

  TScalar Length() const { return std::sqrt(x * x + y * y + z * z); }

  constexpr bool IsZero() const { return (0 == x) && (0 == y) && (0 == z); }

  TScalar DistanceToOrigin() const { return Length(); }

  void Offset(TScalar dx, TScalar dy, TScalar dz) {
    x += dx;
    y += dy;
    z += dz;
  }

  void Scale(TScalar value) {
    x *= value;
    y *= value;
    z *= value;
  }

  bool Normalize() {
    float magSq;
    if (IsLengthNearlyZero(x, y, z, &magSq)) {
      Set(0, 0, 0);
      return false;
    }
    double invScale;
    if (TScalarIsFinite(magSq)) {
      invScale = magSq;
    } else {
      double xx = x;
      double yy = y;
      double zz = z;
      invScale = xx * xx + yy * yy + zz * zz;
    }
    double scale = 1 / sqrt(invScale);
    x *= scale;
    y *= scale;
    z *= scale;
    if (!TScalarIsFinite(x) || !TScalarIsFinite(y) || !TScalarIsFinite(z)) {
      Set(0, 0, 0);
      return false;
    }
    return true;
  }

  TPoint3 operator-() const { return {-x, -y, -z}; }

  TPoint3 operator*(TScalar scale) const { return {x * scale, y * scale, z * scale}; }

  TPoint3& operator*=(TScalar scale) {
    x *= scale;
    y *= scale;
    z *= scale;
    return *this;
  }

  friend TPoint3 operator-(const TPoint3& a, const TPoint3& b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }

  friend TPoint3 operator+(const TPoint3& a, const TPoint3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }

  void operator-=(const TPoint3& p) {
    x -= p.x;
    y -= p.y;
    z -= p.z;
  }

  void operator+=(const TPoint3& p) {
    x += p.x;
    y += p.y;
    z += p.z;
  }

  friend bool operator==(const TPoint3& a, const TPoint3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

  friend bool operator!=(const TPoint3& a, const TPoint3& b) { return !(a == b); }

  static TScalar DotProduct(const TPoint3& a, const TPoint3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

  TScalar Dot(const TPoint3& vec) const { return DotProduct(*this, vec); }

  static TPoint3 CrossProduct(const TPoint3& a, const TPoint3& b) {
    TPoint3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
  }

  TPoint3 Cross(const TPoint3& vec) const { return CrossProduct(*this, vec); }

  bool IsFinite() const {
    TScalar accum = 0;
    accum *= x;
    accum *= y;
    accum *= z;
    assert(0 == accum || accum != accum);
    return accum == accum;
  }

  static inline float GetLengthSquared(float x, float y, float z) { return x * x + y * y + z * z; }

  static inline bool IsLengthNearlyZero(float x, float y, float z, float* lengthSquared) {
    *lengthSquared = GetLengthSquared(x, y, z);
    return *lengthSquared <= (T_ScalarNearlyZero * T_ScalarNearlyZero);
  }
};
}  // namespace tdfcore
