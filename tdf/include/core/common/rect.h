//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <algorithm>
#include <utility>

#include "core/common/point.h"
#include "core/common/size.h"

namespace tdfcore {

struct TRect {
  TScalar left = 0;
  TScalar top = 0;
  TScalar right = 0;
  TScalar bottom = 0;

  static constexpr TRect MakeEmpty() { return {0, 0, 0, 0}; }

  static constexpr TRect MakeWH(TScalar w, TScalar h) { return TRect{0, 0, w, h}; }

  static constexpr TRect MakeSize(const TSize &size) { return {0, 0, size.width, size.height}; }

  static constexpr TRect MakeLTRB(TScalar l, TScalar t, TScalar r, TScalar b) { return {l, t, r, b}; }

  static constexpr TRect MakeXYWH(TScalar x, TScalar y, TScalar w, TScalar h) { return {x, y, x + w, y + h}; }

  bool IsFinite() const { return !std::isnan(left * top * right * bottom); }

  bool IsSorted() const { return left <= right && top <= bottom; }

  bool IsEmpty() const { return !(left < right && top < bottom); }

  TScalar X() const { return left; }

  TScalar Y() const { return top; }

  TScalar Width() const { return right - left; }

  TScalar Height() const { return bottom - top; }

  TScalar centerX() const { return left / 2 + right / 2; }

  TScalar centerY() const { return top / 2 + bottom / 2; }

  friend bool operator==(const TRect &a, const TRect &b) {
    return a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom;
  }

  friend bool operator!=(const TRect &a, const TRect &b) {
    return !(a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom);
  }

  void SetLTRB(TScalar l, TScalar t, TScalar r, TScalar b) {
    left = l;
    top = t;
    right = r;
    bottom = b;
  }

  void SetXYWH(TScalar x, TScalar y, TScalar width, TScalar height) {
    left = x;
    top = y;
    right = x + width;
    bottom = y + height;
  }

  bool SetBounds(const TPoint pts[], int count) {
    if (count <= 0) {
      left = top = right = bottom = 0;
      return true;
    }
    float min_x, max_x;
    float min_y, max_y;
    min_x = max_x = pts[0].x;
    min_y = max_y = pts[0].y;
    for (int i = 1; i < count; i++) {
      auto x = pts[i].x;
      auto y = pts[i].y;
      auto isFinite = ((x + y) * 0 == 0);
      if (!isFinite) {
        left = top = right = bottom = 0;
        return false;
      }
      if (x < min_x) {
        min_x = x;
      }
      if (x > max_x) {
        max_x = x;
      }
      if (y < min_y) {
        min_y = y;
      }
      if (y > max_y) {
        max_y = y;
      }
    }
    SetLTRB(min_x, min_y, max_x, max_y);
    return true;
  }

  constexpr TRect MakeOffset(TScalar dx, TScalar dy) const {
    return MakeLTRB(left + dx, top + dy, right + dx, bottom + dy);
  }

  constexpr TRect MakeOffset(Vector v) const { return this->MakeOffset(v.x, v.y); }

  TRect MakeInset(TScalar dx, TScalar dy) const { return MakeLTRB(left + dx, top + dy, right - dx, bottom - dy); }

  TRect MakeOutset(TScalar dx, TScalar dy) const { return MakeLTRB(left - dx, top - dy, right + dx, bottom + dy); }

  void Offset(TScalar dx, TScalar dy) {
    left += dx;
    top += dy;
    right += dx;
    bottom += dy;
  }

  void Offset(const TPoint &delta) { this->Offset(delta.x, delta.y); }

  void Scale(TScalar scaleX, TScalar scaleY) {
    left *= scaleX;
    right *= scaleX;
    top *= scaleY;
    bottom *= scaleY;
  }

  void Inset(TScalar dx, TScalar dy) {
    left += dx;
    top += dy;
    right -= dx;
    bottom -= dy;
  }

  void Outset(TScalar dx, TScalar dy) { this->Inset(-dx, -dy); }

  bool Contains(TScalar x, TScalar y) const { return x >= left && x < right && y >= top && y < bottom; }

  bool Contains(const TRect &r) const {
    return !r.IsEmpty() && !this->IsEmpty() && left <= r.left && top <= r.top && right >= r.right && bottom >= r.bottom;
  }

  bool IsIntersectsWith(const TRect &rect) const {
    TScalar l = std::max(rect.left, left);
    TScalar r = std::min(rect.right, right);
    TScalar t = std::max(rect.top, top);
    TScalar b = std::min(rect.bottom, bottom);
    return l < r && t < b;
  }

  bool Intersect(const TRect &r) {
#define CHECK_INTERSECT(al, at, ar, ab, bl, bt, br, bb) \
  TScalar L = std::max(al, bl);                        \
  TScalar R = std::min(ar, br);                        \
  TScalar T = std::max(at, bt);                        \
  TScalar B = std::min(ab, bb);                        \
  do {                                                  \
    if (!(L < R && T < B)) return false;                \
  } while (0)

    CHECK_INTERSECT(r.left, r.top, r.right, r.bottom, left, top, right, bottom);
    this->SetLTRB(L, T, R, B);
#undef CHECK_INTERSECT
    return true;
  }

  void Join(const TRect &rect) {
    if (rect.IsEmpty()) {
      return;
    }

    if (this->IsEmpty()) {
      *this = rect;
    } else {
      left = std::min(left, rect.left);
      top = std::min(top, rect.top);
      right = std::max(right, rect.right);
      bottom = std::max(bottom, rect.bottom);
    }
  }

  void RoundOut(TRect * dst) {
    dst->SetLTRB(TScalarFloorToScalar(left), TScalarFloorToScalar(top),
                 TScalarCeilToScalar(right), TScalarCeilToScalar(bottom));
  }

  void Sort() {
    using std::swap;
    if (left > right) {
      swap(left, right);
    }

    if (top > bottom) {
      swap(top, bottom);
    }
  }

  void JoinPossiblyEmptyRect(const TRect& r) {
    left = std::min(left, r.left);
    top = std::min(top, r.top);
    right = std::max(right, r.right);
    bottom = std::max(bottom, r.bottom);
  }
};

struct IRect {
  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;

  static constexpr IRect MakeEmpty() { return IRect{0, 0, 0, 0}; }

  static constexpr IRect MakeWH(int32_t w, int32_t h) { return IRect{0, 0, w, h}; }

  static constexpr IRect MakeSize(const ISize& size) { return IRect{0, 0, size.width, size.height}; }

  static constexpr IRect MakeLTRB(int32_t l, int32_t t,
                                                          int32_t r, int32_t b) {
      return IRect{l, t, r, b};
  }

  static constexpr IRect MakeXYWH(int32_t x, int32_t y,
                                                          int32_t w, int32_t h) {
      return { x, y, x + w, y + h };
  }

  constexpr int32_t x() const { return left; }

  constexpr int32_t y() const { return top; }

  constexpr int32_t width() const { return right - left; }

  constexpr int32_t height() const { return bottom - top; }

  constexpr ISize size() const { return ISize::Make(this->width(), this->height()); }

  void Join(const IRect &r) {
    if (r.left >= r.right || r.top >= r.bottom) {
      return;
    }
    if (left >= right || top >= bottom) {
      *this = r;
    } else {
      if (r.left < left)  left = r.left;
      if (r.top < top)  top = r.top;
      if (r.right > right) right = r.right;
      if (r.bottom > bottom) bottom = r.bottom;
    }
  }
};

}  // namespace tdfcore
