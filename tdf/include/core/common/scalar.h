//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include <stdint.h>
#include <cmath>
#include <limits>
#include <algorithm>
#include <cassert>
#include "core/common/constants.h"

namespace tdfcore {

using TScalar = float;
static constexpr TScalar kTScalarMax = std::numeric_limits<TScalar>::max();
static constexpr TScalar kTScalarZero = static_cast<TScalar>(0);
static constexpr TScalar kNearlyZero = static_cast<TScalar>(1.0 / (1 << 8));

template <typename T>
static constexpr const T& TPin(const T& x, const T& lo, const T& hi) {
    return std::max(lo, std::min(x, hi));
}

template <typename T>
static inline TScalar CastToTScalar(T in) {
  return static_cast<TScalar>(in);
}

#define TMaxS32FitsInFloat    2147483520
#define TMaxS64FitsInFloat    (INT64_MAX >> (63-24) << (63-24))   // 0x7fffff8000000000
#define TMinS64FitsInFloat    -TMaxS64FitsInFloat
#define TFloatRoundToInt(x)   TFloatSaturateToInt(floorf((x) + 0.5f))

static inline int TFloatSaturateToInt(float x) {
  x = x < TMaxS32FitsInFloat ? x : TMaxS32FitsInFloat;
  x = x > TMinS64FitsInFloat ? x : TMinS64FitsInFloat;
  return static_cast<int>(x);
}

#define T_Scalar1 1.0f
#define T_ScalarHalf 0.5f
#define T_ScalarNearlyZero (T_Scalar1 / (1 << 12))
#define DegreesToRadians(degrees) static_cast<TScalar>(((degrees) * TDF_PI) / 180)
#define RadiansToDegrees(radians) static_cast<TScalar>(((radians) * 180) / TDF_PI)

#define TScalarFloorToScalar(x) floorf(x)
#define TScalarCeilToScalar(x) ceilf(x)
#define TScalarRoundToScalar(x)    floorf((x) + 0.5f)
#define TScalarTruncToScalar(x)    truncf(x)
#define TScalarRoundToInt(x)       TFloatRoundToInt(x)

#define TScalarNaN std::numeric_limits<float>::quiet_NaN()
#define TScalarInfinity (+std::numeric_limits<float>::infinity())
#define TScalarNegativeInfinity (-std::numeric_limits<float>::infinity())

#define TScalarSin(x)         sinf(x)
#define TScalarCos(x)         cosf(x)

#define TScalarSqrt(x) sqrtf(x)
#define TScalarSqrt2 1.41421356f;
#define TScalarAbs(x) fabsf(x)
#define TIntToScalar(x) static_cast<TScalar>(x)

#define TScalarHalf(a) ((a) * T_ScalarHalf)
#define TScalarMax 3.402823466e+38f

static inline bool TScalarNearlyZero(TScalar x, TScalar tolerance = static_cast<TScalar>(kNearlyZero)) {
  return fabs(x) <= tolerance;
}

static inline bool TScalarsNearlyZero(const TScalar x[], int n, TScalar tolerance = static_cast<TScalar>(kNearlyZero)) {
  assert(n >= 0);
  for (int i = 0; i < n; ++i) {
    if (!TScalarNearlyZero(x[i])) {
      return false;
    }
  }
  return true;
}

static inline bool TScalarNearlyEqual(TScalar x, TScalar y, TScalar tolerance = static_cast<TScalar>(kNearlyZero)) {
  return fabs(x - y) <= tolerance;
}

static inline bool TScalarsEqual(const TScalar a[], const TScalar b[], int n) {
  assert(n >= 0);
  for (int i = 0; i < n; ++i) {
    if (a[i] != b[i]) {
      return false;
    }
  }
  return true;
}

static inline TScalar SinSnapToZero(TScalar radians) {
  auto v = static_cast<TScalar>(sin(radians));
  return TScalarNearlyZero(v) ? 0.0 : v;
}

static inline TScalar CosSnapToZero(TScalar radians) {
  auto v = static_cast<TScalar>(cos(radians));
  return TScalarNearlyZero(v) ? 0.0 : v;
}

static inline bool TScalarsAreFinite(const TScalar arr[], int count) {
  TScalar p = 0;
  for (int i = 0; i < count; ++i) {
    p *= arr[i];
  }
  return p == 0;
}

static inline bool TScalarIsFinite(TScalar x) {
  return std::isfinite(x);
}

static inline TScalar TScalarSquare(TScalar x) { return x * x; }

}  // namespace tdfcore
