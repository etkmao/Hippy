//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include <cmath>

#include "core/common/scalar.h"
namespace tdfcore {
struct TV3 {
  TScalar x = 0;
  TScalar y = 0;
  TScalar z = 0;

  bool operator==(const TV3& v) const { return x == v.x && y == v.y && z == v.z; }
  bool operator!=(const TV3& v) const { return !(*this == v); }

  TScalar Dot(const TV3& a, const TV3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
  TV3 Cross(const TV3& a, const TV3& b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
  }
  TV3 Normalize(const TV3& v) { return v * (1.0f / v.Length()); }

  TV3 operator-() const { return {-x, -y, -z}; }
  TV3 operator+(const TV3& v) const { return {x + v.x, y + v.y, z + v.z}; }
  TV3 operator-(const TV3& v) const { return {x - v.x, y - v.y, z - v.z}; }

  TV3 operator*(const TV3& v) const { return {x * v.x, y * v.y, z * v.z}; }
  friend TV3 operator*(const TV3& v, TScalar s) { return {v.x * s, v.y * s, v.z * s}; }
  friend TV3 operator*(TScalar s, const TV3& v) { return v * s; }

  void operator+=(TV3 v) { *this = *this + v; }
  void operator-=(TV3 v) { *this = *this - v; }
  void operator*=(TV3 v) { *this = *this * v; }
  void operator*=(TScalar s) { *this = *this * s; }

  TScalar LengthSquared() const { return Dot(*this, *this); }
  TScalar Length() const { return TScalarSquare(Dot(*this, *this)); }

  TScalar Dot(const TV3& v) const { return Dot(*this, v); }
  TV3 Cross(const TV3& v) const { return Cross(*this, v); }
  TV3 Normalize() const { return Normalize(*this); }

  const TScalar* ptr() const { return &x; }
  TScalar* ptr() { return &x; }
};

struct TV4 {
  TScalar x = 0;
  TScalar y = 0;
  TScalar z = 0;
  TScalar w = 0;

  bool operator==(const TV4& v) const {
    return x == v.x && y == v.y && z == v.z && w == v.w;
  }
  bool operator!=(const TV4& v) const { return !(*this == v); }

  TV4 operator-() const { return {-x, -y, -z, -w}; }
  TV4 operator+(const TV4& v) const { return { x + v.x, y + v.y, z + v.z, w + v.w }; }
  TV4 operator-(const TV4& v) const { return { x - v.x, y - v.y, z - v.z, w - v.w }; }

  TV4 operator*(const TV4& v) const {
    return { x*v.x, y*v.y, z*v.z, w*v.w };
  }
  friend TV4 operator*(const TV4& v, TScalar s) {
    return { v.x*s, v.y*s, v.z*s, v.w*s };
  }
  friend TV4 operator*(TScalar s, const TV4& v) { return v*s; }

  const TScalar* ptr() const { return &x; }
  TScalar* ptr() { return &x; }

  TScalar operator[](int i) const {
    assert(i >= 0 && i < 4);
    return this->ptr()[i];
  }
  TScalar& operator[](int i) {
    assert(i >= 0 && i < 4);
    return this->ptr()[i];
  }
};
}  // namespace tdfcore
