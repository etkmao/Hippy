//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <cassert>
#include <cstdint>
#include "core/common/scalar.h"

namespace tdfcore {
typedef uint8_t Alpha;
static constexpr Alpha kAlphaOpaque = 0xFF;

enum class ColorChannel {
    kR,  // the red channel
    kG,  // the green channel
    kB,  // the blue channel
    kA,  // the alpha channel

    kLastEnum = kA,
};

struct Color {
  uint32_t data = 0;
  static constexpr Color ARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    static_assert(sizeof(Color) == sizeof(uint32_t));
    assert(a <= 255 && r <= 255 && g <= 255 && b <= 255);
    return {((uint32_t)a << 24) | (r << 16) | (g << 8) | (b << 0)};
  }

  static constexpr Color MakeRGB(uint8_t r, uint8_t g, uint8_t b) { return Color::ARGB(255, r, g, b); }

  constexpr uint8_t GetA() const { return (data >> 24) & 0xFF; }
  constexpr uint8_t GetR() const { return (data >> 16) & 0xFF; }
  constexpr uint8_t GetG() const { return (data >> 8) & 0xFF; }
  constexpr uint8_t GetB() const { return (data >> 0) & 0xFF; }

  Color& SetA(uint8_t a) {
    data = (data & 0x00FFFFFF) | (a << 24);
    return *this;
  }

  static void RGBToHSV(uint8_t r, uint8_t g, uint8_t b, TScalar hsv[3]) {
    assert(hsv);
    unsigned min = std::min(r, std::min(g, b));
    unsigned max = std::max(r, std::max(g, b));
    unsigned delta = max - min;

    TScalar v = ByteToScalar(max);
    assert(v >= 0 && v <= T_Scalar1);

    if (0 == delta) {
      hsv[0] = 0;
      hsv[1] = 0;
      hsv[2] = v;
      return;
    }
    TScalar s = ByteDivToScalar(delta, max);
    assert(s >= 0 && s <= T_Scalar1);

    TScalar h;
    if (r == max) {
      h = ByteDivToScalar(g - b, delta);
    } else if (g == max) {
      h = TIntToScalar(2) + ByteDivToScalar(b - r, delta);
    } else {
      h = TIntToScalar(4) + ByteDivToScalar(r - g, delta);
    }

    h *= 60;
    if (h < 0) {
        h += TIntToScalar(360);
    }
    assert(h >= 0 && h < TIntToScalar(360));

    hsv[0] = h;
    hsv[1] = s;
    hsv[2] = v;
  }

  static Color HSVToColor(uint8_t a, const TScalar hsv[3]) {
    assert(hsv);

    TScalar s = TPin(hsv[1], 0.0f, 1.0f);
    TScalar v = TPin(hsv[2], 0.0f, 1.0f);

    uint8_t v_byte = TScalarRoundToInt(v * 255);

    if (TScalarNearlyZero(s)) {
        return Color::ARGB(a, v_byte, v_byte, v_byte);
    }
    TScalar hx = (hsv[0] < 0 || hsv[0] >= TIntToScalar(360)) ? 0 : hsv[0]/60;
    TScalar w = TScalarFloorToScalar(hx);
    TScalar f = hx - w;

    unsigned p = TScalarRoundToInt((T_Scalar1 - s) * v * 255);
    unsigned q = TScalarRoundToInt((T_Scalar1 - (s * f)) * v * 255);
    unsigned t = TScalarRoundToInt((T_Scalar1 - (s * (T_Scalar1 - f))) * v * 255);

    unsigned r, g, b;

    assert((unsigned)(w) < 6);
    switch ((unsigned)(w)) {
        case 0: r = v_byte;  g = t;      b = p; break;
        case 1: r = q;       g = v_byte; b = p; break;
        case 2: r = p;       g = v_byte; b = t; break;
        case 3: r = p;       g = q;      b = v_byte; break;
        case 4: r = t;       g = p;      b = v_byte; break;
        default: r = v_byte; g = p;      b = q; break;
    }
    return Color::ARGB(a, r, g, b);
  }

  friend bool operator==(const Color& lhs, const Color& rhs) { return lhs.data == rhs.data; }

  friend bool operator!=(const Color& lhs, const Color& rhs) { return lhs.data != rhs.data; }

  static constexpr Color Transparent() { return {0x00000000}; }
  static constexpr Color Black() { return {0xFF000000}; }
  static constexpr Color DKGray() { return {0xFF444444}; }
  static constexpr Color Gray() { return {0xFF888888}; }
  static constexpr Color LTGray() { return {0xFFCCCCCC}; }
  static constexpr Color White() { return {0xFFFFFFFF}; }
  static constexpr Color Red() { return {0xFFFF0000}; }
  static constexpr Color Green() { return {0xFF00FF00}; }
  static constexpr Color Blue() { return {0xFF0000FF}; }
  static constexpr Color Yellow() { return {0xFFFFFF00}; }
  static constexpr Color Cyan() { return {0xFF00FFFF}; }
  static constexpr Color Magenta() { return {0xFFFF00FF}; }

  static inline TScalar ByteToScalar(uint8_t x) {
      assert(x <= 255);
      return static_cast<TScalar>(x) / 255;
  }

  static inline TScalar ByteDivToScalar(int numer, uint8_t denom) {
      return static_cast<TScalar>(numer) / static_cast<int>(denom);
  }
};

}  // namespace tdfcore
