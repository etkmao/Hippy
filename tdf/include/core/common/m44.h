//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <cstring>
#include <sstream>
#include "core/common/m33.h"
#include "core/common/rect.h"
#include "core/common/scalar.h"

namespace tdfcore {

struct TV2 {
  float x, y;

  bool operator==(const TV2 v) const { return x == v.x && y == v.y; }
  bool operator!=(const TV2 v) const { return !(*this == v); }

  static TScalar Dot(TV2 a, TV2 b) { return a.x * b.x + a.y * b.y; }
  static TScalar Cross(TV2 a, TV2 b) { return a.x * b.y - a.y * b.x; }
  static TV2 Normalize(TV2 v) { return v * (1.0f / v.length()); }

  TV2 operator-() const { return {-x, -y}; }
  TV2 operator+(TV2 v) const { return {x + v.x, y + v.y}; }
  TV2 operator-(TV2 v) const { return {x - v.x, y - v.y}; }

  TV2 operator*(TV2 v) const { return {x * v.x, y * v.y}; }
  friend TV2 operator*(TV2 v, TScalar s) { return {v.x * s, v.y * s}; }
  friend TV2 operator*(TScalar s, TV2 v) { return {v.x * s, v.y * s}; }
  friend TV2 operator/(TV2 v, TScalar s) { return {v.x / s, v.y / s}; }

  void operator+=(TV2 v) { *this = *this + v; }
  void operator-=(TV2 v) { *this = *this - v; }
  void operator*=(TV2 v) { *this = *this * v; }
  void operator*=(TScalar s) { *this = *this * s; }
  void operator/=(TScalar s) { *this = *this / s; }

  TScalar lengthSquared() const { return Dot(*this, *this); }
  TScalar length() const { return TScalarSqrt(this->lengthSquared()); }

  TScalar dot(TV2 v) const { return Dot(*this, v); }
  TScalar cross(TV2 v) const { return Cross(*this, v); }
  TV2 normalize() const { return Normalize(*this); }

  const float* ptr() const { return &x; }
  float* ptr() { return &x; }
};

struct TV3 {
  float x, y, z;

  bool operator==(const TV3& v) const { return x == v.x && y == v.y && z == v.z; }
  bool operator!=(const TV3& v) const { return !(*this == v); }

  static TScalar Dot(const TV3& a, const TV3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
  static TV3 Cross(const TV3& a, const TV3& b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
  }
  static TV3 Normalize(const TV3& v) { return v * (1.0f / v.length()); }

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

  TScalar lengthSquared() const { return Dot(*this, *this); }
  TScalar length() const { return TScalarSqrt(Dot(*this, *this)); }

  TScalar dot(const TV3& v) const { return Dot(*this, v); }
  TV3 cross(const TV3& v) const { return Cross(*this, v); }
  TV3 normalize() const { return Normalize(*this); }

  const float* ptr() const { return &x; }
  float* ptr() { return &x; }
};

struct TV4 {
  float x, y, z, w;

  bool operator==(const TV4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
  bool operator!=(const TV4& v) const { return !(*this == v); }

  TV4 operator-() const { return {-x, -y, -z, -w}; }
  TV4 operator+(const TV4& v) const { return {x + v.x, y + v.y, z + v.z, w + v.w}; }
  TV4 operator-(const TV4& v) const { return {x - v.x, y - v.y, z - v.z, w - v.w}; }

  TV4 operator*(const TV4& v) const { return {x * v.x, y * v.y, z * v.z, w * v.w}; }
  friend TV4 operator*(const TV4& v, TScalar s) { return {v.x * s, v.y * s, v.z * s, v.w * s}; }
  friend TV4 operator*(TScalar s, const TV4& v) { return v * s; }

  const float* ptr() const { return &x; }
  float* ptr() { return &x; }

  float operator[](int i) const { return this->ptr()[i]; }
  float& operator[](int i) { return this->ptr()[i]; }
};

/**
 *  4x4 matrix used by SkCanvas and other parts of Skia.
 *
 *  Skia assumes a right-handed coordinate system:
 *      +X goes to the right
 *      +Y goes down
 *      +Z goes into the screen (away from the viewer)
 */
class TM44 {
 public:
  TM44(const TM44& src) = default;
  TM44& operator=(const TM44& src) = default;

  constexpr TM44() : fMat{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1} {}

  TM44(const TM44& a, const TM44& b) { this->setConcat(a, b); }

  enum Uninitialized_Constructor { kUninitialized_Constructor };
  explicit TM44(Uninitialized_Constructor) {}

  enum NaN_Constructor { kNaN_Constructor };
  explicit constexpr TM44(NaN_Constructor)
      : fMat{TScalarNaN, TScalarNaN, TScalarNaN, TScalarNaN, TScalarNaN, TScalarNaN, TScalarNaN, TScalarNaN,
             TScalarNaN, TScalarNaN, TScalarNaN, TScalarNaN, TScalarNaN, TScalarNaN, TScalarNaN, TScalarNaN} {}

  /**
   *  The constructor parameters are in row-major order.
   */
  constexpr TM44(TScalar m0, TScalar m4, TScalar m8, TScalar m12, TScalar m1, TScalar m5, TScalar m9, TScalar m13,
                 TScalar m2, TScalar m6, TScalar m10, TScalar m14, TScalar m3, TScalar m7, TScalar m11, TScalar m15)
      // fMat is column-major order in memory.
      : fMat{m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15} {}

  static TM44 Rows(const TV4& r0, const TV4& r1, const TV4& r2, const TV4& r3) {
    TM44 m(kUninitialized_Constructor);
    m.setRow(0, r0);
    m.setRow(1, r1);
    m.setRow(2, r2);
    m.setRow(3, r3);
    return m;
  }
  static TM44 Cols(const TV4& c0, const TV4& c1, const TV4& c2, const TV4& c3) {
    TM44 m(kUninitialized_Constructor);
    m.setCol(0, c0);
    m.setCol(1, c1);
    m.setCol(2, c2);
    m.setCol(3, c3);
    return m;
  }

  static TM44 RowMajor(const TScalar r[16]) {
    return TM44(r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15]);
  }
  static TM44 ColMajor(const TScalar c[16]) {
    return TM44(c[0], c[4], c[8], c[12], c[1], c[5], c[9], c[13], c[2], c[6], c[10], c[14], c[3], c[7], c[11], c[15]);
  }

  static TM44 Translate(TScalar x, TScalar y, TScalar z = 0) {
    return TM44(1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1);
  }

  static TM44 Scale(TScalar x, TScalar y, TScalar z = 1) {
    return TM44(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
  }

  static TM44 Rotate(TV3 axis, TScalar radians) {
    TM44 m(kUninitialized_Constructor);
    m.setRotate(axis, radians);
    return m;
  }

  // Scales and translates 'src' to fill 'dst' exactly.
  static TM44 RectToRect(const TRect& src, const TRect& dst);

  static TM44 LookAt(const TV3& eye, const TV3& center, const TV3& up);
  static TM44 Perspective(float near, float far, float angle);

  bool operator==(const TM44& other) const;
  bool operator!=(const TM44& other) const { return !(other == *this); }

  std::string ToString() const {
    std::ostringstream oss;
    oss << "TM44(" << fMat[0] << ", " << fMat[4] << ", " << fMat[8] << ", " << fMat[12] << ", " << fMat[1] << ", "
        << fMat[5] << ", " << fMat[9] << ", " << fMat[13] << ", " << fMat[2] << ", " << fMat[6] << ", " << fMat[10]
        << ", " << fMat[14] << ", " << fMat[3] << ", " << fMat[7] << ", " << fMat[11] << ", " << fMat[15] << ")";
    return oss.str();
  }

  void getColMajor(TScalar v[]) const { memcpy(v, fMat, sizeof(fMat)); }
  void getRowMajor(TScalar v[]) const;

  TScalar rc(int r, int c) const { return fMat[c * 4 + r]; }
  void setRC(int r, int c, TScalar value) { fMat[c * 4 + r] = value; }

  TV4 row(int i) const { return {fMat[i + 0], fMat[i + 4], fMat[i + 8], fMat[i + 12]}; }
  TV4 col(int i) const { return {fMat[i * 4 + 0], fMat[i * 4 + 1], fMat[i * 4 + 2], fMat[i * 4 + 3]}; }

  void setRow(int i, const TV4& v) {
    fMat[i + 0] = v.x;
    fMat[i + 4] = v.y;
    fMat[i + 8] = v.z;
    fMat[i + 12] = v.w;
  }
  void setCol(int i, const TV4& v) { memcpy(&fMat[i * 4], v.ptr(), sizeof(v)); }

  TM44& setIdentity() {
    *this = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    return *this;
  }

  TM44& setTranslate(TScalar x, TScalar y, TScalar z = 0) {
    *this = {1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1};
    return *this;
  }

  TM44& setScale(TScalar x, TScalar y, TScalar z = 1) {
    *this = {x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1};
    return *this;
  }

  /**
   *  Set this matrix to rotate about the specified unit-length axis vector,
   *  by an angle specified by its sin() and cos().
   *
   *  This does not attempt to verify that axis.length() == 1 or that the sin,cos values
   *  are correct.
   */
  TM44& setRotateUnitSinCos(TV3 axis, TScalar sinAngle, TScalar cosAngle);

  /**
   *  Set this matrix to rotate about the specified unit-length axis vector,
   *  by an angle specified in radians.
   *
   *  This does not attempt to verify that axis.length() == 1.
   */
  TM44& setRotateUnit(TV3 axis, TScalar radians) {
    return this->setRotateUnitSinCos(axis, TScalarSin(radians), TScalarCos(radians));
  }

  /**
   *  Set this matrix to rotate about the specified axis vector,
   *  by an angle specified in radians.
   *
   *  Note: axis is not assumed to be unit-length, so it will be normalized internally.
   *        If axis is already unit-length, call setRotateAboutUnitRadians() instead.
   */
  TM44& setRotate(TV3 axis, TScalar radians);

  TM44& setConcat(const TM44& a, const TM44& b);

  friend TM44 operator*(const TM44& a, const TM44& b) { return TM44(a, b); }

  TM44& preConcat(const TM44& m) { return this->setConcat(*this, m); }

  TM44& postConcat(const TM44& m) { return this->setConcat(m, *this); }

  /**
   *  A matrix is categorized as 'perspective' if the bottom row is not [0, 0, 0, 1].
   *  For most uses, a bottom row of [0, 0, 0, X] behaves like a non-perspective matrix, though
   *  it will be categorized as perspective. Calling normalizePerspective() will change the
   *  matrix such that, if its bottom row was [0, 0, 0, X], it will be changed to [0, 0, 0, 1]
   *  by scaling the rest of the matrix by 1/X.
   *
   *  | A B C D |    | A/X B/X C/X D/X |
   *  | E F G H | -> | E/X F/X G/X H/X |   for X != 0
   *  | I J K L |    | I/X J/X K/X L/X |
   *  | 0 0 0 X |    |  0   0   0   1  |
   */
  void normalizePerspective();

  /** Returns true if all elements of the matrix are finite. Returns false if any
      element is infinity, or NaN.

      @return  true if matrix has only finite elements
  */
  bool isFinite() const { return TScalarsAreFinite(fMat, 16); }

  /** If this is invertible, return that in inverse and return true. If it is
   *  not invertible, return false and leave the inverse parameter unchanged.
   */
  bool invert(TM44* inverse) const;

  TM44 transpose() const;

  void dump() const;

  ////////////

  TV4 map(float x, float y, float z, float w) const;
  TV4 operator*(const TV4& v) const { return this->map(v.x, v.y, v.z, v.w); }
  TV3 operator*(TV3 v) const {
    auto v4 = this->map(v.x, v.y, v.z, 0);
    return {v4.x, v4.y, v4.z};
  }
  ////////////////////// Converting to/from SkMatrix

  /* When converting from TMatrix44 to SkMatrix, the third row and
   * column is dropped.  When converting from SkMatrix to TMatrix44
   * the third row and column remain as identity:
   * [ a b c ]      [ a b 0 c ]
   * [ d e f ]  ->  [ d e 0 f ]
   * [ g h i ]      [ 0 0 1 0 ]
   *                [ g h 0 i ]
   */
  TM33 asM33() const {
    return TM33::MakeAll(fMat[0], fMat[4], fMat[12], fMat[1], fMat[5], fMat[13], fMat[3], fMat[7], fMat[15]);
  }

  explicit TM44(const TM33& src)
      : TM44(src[TM33::SCALE_X], src[TM33::SKEW_X], 0, src[TM33::TRANS_X], src[TM33::SKEW_Y], src[TM33::SCALE_Y], 0,
             src[TM33::TRANS_Y], 0, 0, 1, 0, src[TM33::PERSP_0], src[TM33::PERSP_1], 0, src[TM33::PERSP_2]) {}

  TM44& preTranslate(TScalar x, TScalar y, TScalar z = 0);
  TM44& postTranslate(TScalar x, TScalar y, TScalar z = 0);

  TM44& preScale(TScalar x, TScalar y);
  TM44& preScale(TScalar x, TScalar y, TScalar z);
  TM44& preConcat(const TM33&);

 private:
  /* Stored in column-major.
   *  Indices
   *  0  4  8  12        1 0 0 trans_x
   *  1  5  9  13  e.g.  0 1 0 trans_y
   *  2  6 10  14        0 0 1 trans_z
   *  3  7 11  15        0 0 0 1
   */
  TScalar fMat[16];
};

}  // namespace tdfcore
