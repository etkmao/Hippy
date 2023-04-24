//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <algorithm>
#include <utility>
#include <array>

#include "core/common/point.h"
#include "core/common/rect.h"
#include "core/common/size.h"
#include "core/common/m33.h"

namespace tdfcore {

class RRect {
 public:
  enum class Corner {
    kUpperLeft,
    kUpperRight,
    kLowerRight,
    kLowerLeft,
  };

  enum Type {
    kEmptyType,
    kRectType,
    kOvalType,
    kSimpleType,
    kNinePatchType,
    kComplexType,
    kLastType      = kComplexType,
  };

  RRect() = default;
  RRect(const RRect &rrect) = default;
  RRect &operator=(const RRect &rrect) = default;

  Type GetType() const {
    assert(this->IsValid());
    return static_cast<Type>(type_);
  }

  inline bool isEmpty() const { return kEmptyType == this->GetType(); }
  inline bool isRect() const { return kRectType == this->GetType(); }
  inline bool isOval() const { return kOvalType == this->GetType(); }
  inline bool isSimple() const { return kSimpleType == this->GetType(); }
  inline bool isNinePatch() const { return kNinePatchType == this->GetType(); }
  inline bool isComplex() const { return kComplexType == this->GetType(); }

  TScalar Width() const { return rect_.Width(); }
  TScalar Height() const { return rect_.Height(); }

  void SetEmpty() { *this = RRect(); }
  void SetRect(const TRect &rect);
  static RRect MakeEmpty() { return {}; }
  static RRect MakeRect(const TRect &r);
  static RRect MakeOval(const TRect &oval);
  static RRect MakeRectXY(const TRect &rect, TScalar xrad, TScalar yrad);
  bool Contains(const TRect &rect) const;
  bool IsValid() const;
  bool Transform(const TM33& matrix, RRect* dst) const;
  void SetOval(const TRect &oval);
  void SetRectXY(const TRect &rect, TScalar xrad, TScalar yrad);
  void SetRectRadii(const TRect &rect, const std::array<TPoint, 4> radii);

  void SetNinePatch(const TRect& rect, TScalar left_rad, TScalar top_rad,
                    TScalar right_rad, TScalar bottom_rad);

  const TRect &GetRect() const { return rect_; }
  TPoint GetRadii(Corner corner) const { return radii_[static_cast<std::underlying_type<Corner>::type>(corner)]; }
  const TRect &GetBounds() const { return rect_; }

  friend bool operator==(const RRect &a, const RRect &b) {
    return a.rect_ == b.rect_ && TScalarsEqual(&a.radii_[0].x, &b.radii_[0].x, 8);
  }

  friend bool operator!=(const RRect &a, const RRect &b) {
    return a.rect_ != b.rect_ || !TScalarsEqual(&a.radii_[0].x, &b.radii_[0].y, 8);
  }

  void Inset(TScalar dx, TScalar dy, RRect *dst) const;
  void Inset(TScalar dx, TScalar dy) { Inset(dx, dy, this); }
  void Outset(TScalar dx, TScalar dy, RRect *dst) const { Inset(-dx, -dy, dst); }
  void Outset(TScalar dx, TScalar dy) { Inset(-dx, -dy, this); }
  void Offset(TScalar dx, TScalar dy) { rect_.Offset(dx, dy); }
  RRect MakeOffset(TScalar dx, TScalar dy) const { return RRect(rect_.MakeOffset(dx, dy), radii_); }

  static constexpr size_t kSizeInMemory = 12 * sizeof(TScalar);

  RRect(const TRect &rect, const std::array<TPoint, 4> &radii)
      : rect_(rect), radii_{radii[0], radii[1], radii[2], radii[3]} {}

  inline int CornerToIndex(Corner corner) const { return static_cast<std::underlying_type<Corner>::type>(corner); }

 private:
  static bool AreRectAndRadiiValid(const TRect&, const std::array<TPoint, 4> &radii);

  RRect(const TRect& rect, const std::array<TPoint, 4> &radii, int32_t type)
      : rect_(rect)
      , radii_{radii[0], radii[1], radii[2], radii[3]}
      , type_(type) {}

  bool InitializeRect(const TRect &);
  void ComputeType();
  bool CheckCornerContainment(TScalar x, TScalar y) const;
  bool ScaleRadii();
  bool RadiiAreNinePatch(const std::array<TPoint, 4> radii);
  TRect rect_ = TRect::MakeEmpty();
  // Radii order is UL, UR, LR, LL. Use Corner enum to index into radii_[]
  std::array<TPoint, 4> radii_ = {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}};
  int32_t type_ = kEmptyType;
};

}  // namespace tdfcore
