//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include <cstring>
#include "core/common/macro.h"
#include "core/common/point.h"
#include "core/common/rect.h"
#include "core/common/scalar.h"

namespace tdfcore {

enum class ScaleToFit {
  kFill,
  kStart,
  kCenter,
  kEnd
};

/***
 * TM33 holds a 3x3 TM33 for transforming coordinates. This allows mapping Point and vectors
 * with translation, scaling, skewing, rotation, and perspective.
 * TM33 elements are in row major order. TM33 does not have a constructor, so it must be
 * explicitly initialized. setIdentity() initializes TM33 so it has no effect.
 * setTranslate(), setScale(), setSkew(), setRotate(), set9 and setAll() initializes all TM33
 * elements with the corresponding mapping.
 */
class TM33 {
 public:
  TM33();

  /**
   * Sets TM33 to scale by (sx, sy). Returned TM33 is:
   *
   *       | sx  0  0 |
   *       |  0 sy  0 |
   *       |  0  0  1 |
   *
   *  @param sx  horizontal scale factor
   *  @param sy  vertical scale factor
   *  @return    TM33 with scale
   */
  static TM33 Scale(TScalar sx, TScalar sy) {
    TM33 m = {};
    m.SetScale(sx, sy);
    return m;
  }

  /**
   * Sets TM33 to scale by (scale, scale). Returned TM33 is:
   *
   *      | scale   0   0 |
   *      |   0   scale 0 |
   *      |   0     0   1 |
   *
   * @param scale  horizontal and vertical scale factor
   * @return       TM33 with scale
   */
  static TM33 Scale(TScalar scale) {
    TM33 m = {};
    m.SetScale(scale, scale);
    return m;
  }

  /**
   * Sets TM33 to translate by (dx, dy). Returned TM33 is:
   *
   *       | 1 0 dx |
   *       | 0 1 dy |
   *       | 0 0  1 |
   *
   * @param dx  horizontal translation
   * @param dy  vertical translation
   * @return    TM33 with translation
   */
  static TM33 Translate(TScalar dx, TScalar dy) {
    TM33 m = {};
    m.SetTranslate(dx, dy);
    return m;
  }

  static TM33 Rotate(TScalar px, TScalar py, TScalar degree) {
    TM33 m = {};
    m.SetRotate(px, py, degree);
    return m;
  }

  /**
   * Sets TM33 to:
   *
   *      | scaleX  skewX transX |
   *      |  skewY scaleY transY |
   *      |  pers0  pers1  pers2 |
   *
   * @param scaleX  horizontal scale factor
   * @param skewX   horizontal skew factor
   * @param transX  horizontal translation
   * @param skewY   vertical skew factor
   * @param scaleY  vertical scale factor
   * @param transY  vertical translation
   * @param pers0   input x-axis perspective factor
   * @param pers1   input y-axis perspective factor
   * @param pers2   perspective scale factor
   * @return        TM33 constructed from parameters
   */
  static TM33 MakeAll(TScalar scaleX, TScalar skewX, TScalar transX, TScalar skewY, TScalar scaleY, TScalar transY,
                      TScalar pers0, TScalar pers1, TScalar pers2) {
    TM33 m = {};
    m.SetAll(scaleX, skewX, transX, skewY, scaleY, transY, pers0, pers1, pers2);
    return m;
  }

  static TM33 RectToRect(const TRect& src, const TRect& dst, ScaleToFit mode = ScaleToFit::kFill) {
    TM33 m;
    m.SetRectToRect(src, dst, mode);
    return m;
  }

  /**
   * Returns reference to const identity TM33. Returned TM33 is set to:
   *
   *       | 1 0 0 |
   *       | 0 1 0 |
   *       | 0 0 1 |
   *
   *   @return  const identity TM33
   */
  static const TM33& I();

  /**
   * Returns true if TM33 is identity.  Identity TM33 is:
   *
   *       | 1 0 0 |
   *       | 0 1 0 |
   *       | 0 0 1 |
   *
   * @return  true if TM33 has no effect
   */
  bool IsIdentity() const {
    return this->GetType() == 0;
  }

  /**
   * Returns one TM33 value.
   */
  TScalar operator[](int index) const { return values[index]; }

  /**
   * Returns scale factor multiplied by x-axis input, contributing to x-axis output. With
   * mapPoints(), scales Point along the x-axis.
   * @return  horizontal scale factor
   */
  TScalar GetScaleX() const { return values[SCALE_X]; }

  /**
   * Returns scale factor multiplied by y-axis input, contributing to y-axis output. With
   * mapPoints(), scales Point along the y-axis.
   * @return  vertical scale factor
   */
  TScalar GetScaleY() const { return values[SCALE_Y]; }

  /** Returns scale factor multiplied by y-axis input, contributing to x-axis output.
   *  With mapPoints(), skews Point along the x-axis.
   *  Skewing both axes can rotate Point.
   *  @return  horizontal scale factor
   */
  TScalar GetSkewX() const { return values[SKEW_X]; }

  /** Returns scale factor multiplied by x-axis input, contributing to y-axis output.
   *  With mapPoints(), skews Point along the y-axis.
   *  Skewing both axes can rotate Point.
   *  @return  vertical skew factor
   */
  TScalar GetSkewY() const { return values[SKEW_Y]; }

  /**
   * Returns translation contributing to x-axis output. With mapPoints(), moves Point along the
   * x-axis.
   * @return  horizontal translation factor
   */
  TScalar GetTranslateX() const { return values[TRANS_X]; }

  /**
   * Returns translation contributing to y-axis output. With mapPoints(), moves Point along the
   * y-axis.
   * @return  vertical translation factor
   */
  TScalar GetTranslateY() const { return values[TRANS_Y]; }

  /**
   * Returns writable TM33 value.
   */
  TScalar& operator[](int index) { return values[index]; }

  /**
   * Sets horizontal scale factor.
   * @param v  horizontal scale factor to store
   */
  void SetScaleX(TScalar v) { values[SCALE_X] = v; }

  /**
   * Sets vertical scale factor.
   * @param v  vertical scale factor to store
   */
  void SetScaleY(TScalar v) { values[SCALE_Y] = v; }

  /**
   * Sets horizontal translation.
   * @param v  horizontal translation to store
   */
  void SetTranslateX(TScalar v) { values[TRANS_X] = v; }

  /**
   * Sets vertical translation.
   * @param v  vertical translation to store
   */
  void SetTranslateY(TScalar v) { values[TRANS_Y] = v; }

  /**
   * Sets all values from parameters. Sets TM33 to:
   *
   *      | scaleX  skewX transX |
   *      |  skewY scaleY transY |
   *      | persp0 persp1 persp2 |
   *
   * @param scaleX  horizontal scale factor to store
   * @param skewX   horizontal skew factor to store
   * @param transX  horizontal translation to store
   * @param skewY   vertical skew factor to store
   * @param scaleY  vertical scale factor to store
   * @param transY  vertical translation to store
   * @param persp0  input x-axis values perspective factor to store
   * @param persp1  input y-axis values perspective factor to store
   * @param persp2  perspective scale factor to store
   */
  void SetAll(TScalar scaleX, TScalar skewX, TScalar transX, TScalar skewY, TScalar scaleY, TScalar transY,
              TScalar persp0, TScalar persp1, TScalar persp2);

  void SetAffine(TScalar a, TScalar b, TScalar c, TScalar d, TScalar tx, TScalar ty);

  /**
   * Copies nine scalar values contained by TM33 into buffer, in member value ascending order:
   * ScaleX, SkewX, TransX, SkewY, ScaleY, TransY, Persp0, Persp1, Persp2.
   * @param buffer  storage for nine scalar values
   */
  void Get9(TScalar buffer[9]) const { memcpy(buffer, values, 9 * sizeof(TScalar)); }

  /**
   * Sets TM33 to nine scalar values in buffer, in member value ascending order: ScaleX,
   * SkewX, TransX, SkewY, ScaleY, TransY, Persp0, Persp1, Persp2.
   *
   * Sets TM33 to:
   *
   *     | buffer[0] buffer[1] buffer[2] |
   *     | buffer[3] buffer[4] buffer[5] |
   *     | buffer[6] buffer[7] buffer[8] |
   *
   * @param buffer  nine scalar values
   */
  void Set9(const TScalar buffer[9]) { memcpy(values, buffer, 9 * sizeof(TScalar)); }

  /**
   * Sets TM33 to identity; which has no effect on mapped Point. Sets TM33 to:
   *
   *       | 1 0 0 |
   *       | 0 1 0 |
   *       | 0 0 1 |
   *
   * Also called setIdentity(); use the one that provides better inline documentation.
   */
  void Reset();

  /**
   * Sets TM33 to identity; which has no effect on mapped Point. Sets TM33 to:
   *
   *       | 1 0 0 |
   *       | 0 1 0 |
   *       | 0 0 1 |
   *
   *  Also called reset(); use the one that provides better inline documentation.
   */
  void SetIdentity();

  /**
   * Sets TM33 to translate by (dx, dy).
   * @param dx  horizontal translation
   * @param dy  vertical translation
   */
  TM33& SetTranslate(TScalar dx, TScalar dy);

  /**
   * Sets TM33 to scale by sx and sy, about a pivot point at (px, py). The pivot point is
   * unchanged when mapped with TM33.
   * @param sx  horizontal scale factor
   * @param sy  vertical scale factor
   * @param px  pivot on x-axis
   * @param py  pivot on y-axis
   */
  void SetScale(TScalar sx, TScalar sy, TScalar px, TScalar py);

  /**
   * Sets TM33 to scale by sx and sy about at pivot point at (0, 0).
   * @param sx  horizontal scale factor
   * @param sy  vertical scale factor
   */
  TM33& SetScale(TScalar sx, TScalar sy);

  /**
   * Sets TM33 to rotate by degree about a pivot point at (px, py). The pivot point is
   * unchanged when mapped with TM33. Positive degree rotates clockwise.
   *  @param degree  degreeius of axes relative to upright axes
   *  @param px       pivot on x-axis
   *  @param py       pivot on y-axis
   */
  TM33& SetRotate(TScalar px, TScalar py, TScalar degree);

  /**
   * Sets TM33 to rotate by degree about a pivot point at (0, 0). Positive degree rotates
   * clockwise.
   * @param degree degreeius of axes relative to upright axes
   */
  TM33& SetRotate(TScalar degree);

  bool SetRectToRect(const TRect &src, const TRect &dst, ScaleToFit scale_mode);

  /**
   * Sets TM33 to rotate by sinValue and cosValue, about a pivot point at (px, py).
   * The pivot point is unchanged when mapped with TM33.
   * Vector (sinValue, cosValue) describes the angle of rotation relative to (0, 1).
   * Vector length specifies scale.
   */
  void SetSinCos(TScalar sinV, TScalar cosV, TScalar px, TScalar py);

  /**
   * Sets TM33 to rotate by sinValue and cosValue, about a pivot point at (0, 0).
   * Vector (sinValue, cosValue) describes the angle of rotation relative to (0, 1).
   * Vector length specifies scale.
   */
  void SetSinCos(TScalar sinV, TScalar cosV);

  /**
   * Sets TM33 to TM33 a multiplied by TM33 b. Either a or b may be this.
   *
   * Given:
   *
   *          | A B C |      | J K L |
   *      a = | D E F |, b = | M N O |
   *          | G H I |      | P Q R |
   *
   * sets TM33 to:
   *
   *              | A B C |   | J K L |   | AJ+BM+CP AK+BN+CQ AL+BO+CR |
   *      a * b = | D E F | * | M N O | = | DJ+EM+FP DK+EN+FQ DL+EO+FR |
   *              | G H I |   | P Q R |   | GJ+HM+IP GK+HN+IQ GL+HO+IR |
   *
   * @param a  TM33 on left side of multiply expression
   * @param b  TM33 on right side of multiply expression
   */
  void SetConcat(const TM33& a, const TM33& b);

  /**
   * Preconcats the TM33 with the specified scale. M' = M * S(sx, sy)
   */
  TM33& PreTranslate(TScalar dx, TScalar dy);

  /**
   * Postconcats the TM33 with the specified scale. M' = S(sx, sy, px, py) * M
   */
  void PreScale(TScalar sx, TScalar sy, TScalar px, TScalar py);

  /**
   * Preconcats the TM33 with the specified scale. M' = M * S(sx, sy)
   */
  TM33& PreScale(TScalar sx, TScalar sy);

  /**
   * Preconcats the TM33 with the specified rotation. M' = M * R(degree, px, py)
   */
  TM33& PreRotate(TScalar px, TScalar py, TScalar degree);

  /**
   * Preconcats the TM33 with the specified rotation. M' = M * R(degree)
   */
  TM33& PreRotate(TScalar degree);

  /**
   * Preconcats the TM33 with the specified TM33. M' = M * other
   */
  TM33& PreConcat(const TM33& other);

  /**
   * Postconcats the TM33 with the specified translation. M' = T(dx, dy) * M
   */
  TM33& PostTranslate(TScalar dx, TScalar dy);

  /**
   * Postconcats the TM33 with the specified scale. M' = S(sx, sy, px, py) * M
   */
  TM33& PostScale(TScalar sx, TScalar sy, TScalar px, TScalar py);

  /**
   * Postconcats the TM33 with the specified scale. M' = S(sx, sy) * M
   */
  TM33& PostScale(TScalar sx, TScalar sy);

  /**
   * Postconcats the TM33 with the specified rotation. M' = R(degree, px, py) * M
   */
  TM33& PostRotate(TScalar px, TScalar py, TScalar degree);

  /**
   * Postconcats the TM33 with the specified rotation. M' = R(degree) * M
   */
  TM33& PostRotate(TScalar degree);

  /**
   * Postconcats the TM33 with the specified TM33. M' = other * M
   */
  TM33& PostConcat(const TM33& other);

  /**
   * If this TM33 can be inverted, return true and if inverse is not null, set inverse to be the
   * inverse of this TM33. If this TM33 cannot be inverted, ignore inverse and return false.
   */
  bool Invert(TM33* inverse) const {
    if (this->IsIdentity()) {
      if (inverse) {
        inverse->Reset();
      }
      return true;
    }
    return this->InvertNonIdentity(inverse);
  }

  bool Invertible() const;

  /**
   * Maps src Point array of length count to dst Point array of equal or greater length. Point are
   * mapped by multiplying each Point by TM33. Given:
   *
   *                | A B C |        | x |
   *       TM33 = | D E F |,  pt = | y |
   *                | G H I |        | 1 |
   *
   * where
   *
   *       for (i = 0; i < count; ++i) {
   *           x = src[i].fX
   *           y = src[i].fY
   *       }
   *
   * each dst Point is computed as:
   *
   *                     |A B C| |x|                               Ax+By+C   Dx+Ey+F
   *       TM33 * pt = |D E F| |y| = |Ax+By+C Dx+Ey+F Gx+Hy+I| = ------- , -------
   *                     |G H I| |1|                               Gx+Hy+I   Gx+Hy+I
   *
   * src and dst may point to the same storage.
   *
   * @param dst    storage for mapped Point
   * @param src    Point to transform
   * @param count  number of Point to transform
   */
  void MapPoints(TPoint dst[], const TPoint src[], int count) const;

  /**
   * Maps pts Point array of length count in place. Point are mapped by multiplying each Point by
   * TM33. Given:
   *
   *                 | A B C |        | x |
   *        TM33 = | D E F |,  pt = | y |
   *                 | G H I |        | 1 |
   *
   * where
   *
   *        for (i = 0; i < count; ++i) {
   *            x = pts[i].fX
   *            y = pts[i].fY
   *        }
   *
   * each resulting pts Point is computed as:
   *
   *                      |A B C| |x|                               Ax+By+C   Dx+Ey+F
   *        TM33 * pt = |D E F| |y| = |Ax+By+C Dx+Ey+F Gx+Hy+I| = ------- , -------
   *                      |G H I| |1|                               Gx+Hy+I   Gx+Hy+I
   *
   * @param pts    storage for mapped Point
   * @param count  number of Point to transform
   */
  void MapPoints(TPoint pts[], int count) const { this->MapPoints(pts, pts, count); }

  /** Returns Point pt multiplied by TM33. Given:

                     | A B C |        | x |
            Matrix = | D E F |,  pt = | y |
                     | G H I |        | 1 |

        result is computed as:

                          |A B C| |x|                               Ax+By+C   Dx+Ey+F
            Matrix * pt = |D E F| |y| = |Ax+By+C Dx+Ey+F Gx+Hy+I| = ------- , -------
                          |G H I| |1|                               Gx+Hy+I   Gx+Hy+I

        @param p  Point to map
        @return   mapped Point
    */
  TPoint MapPoint(TPoint pt) const {
    TPoint result;
    this->MapXY(pt.x, pt.y, &result);
    return result;
  }

  /**
   * Maps Point (x, y) to result. Point is mapped by multiplying by Matrix. Given:
   *
   *                | A B C |        | x |
   *       Matrix = | D E F |,  pt = | y |
   *                | G H I |        | 1 |
   *
   * result is computed as:
   *
   *                     |A B C| |x|                               Ax+By+C   Dx+Ey+F
   *       Matrix * pt = |D E F| |y| = |Ax+By+C Dx+Ey+F Gx+Hy+I| = ------- , -------
   *                     |G H I| |1|                               Gx+Hy+I   Gx+Hy+I
   *
   * @param x       x-axis value of Point to map
   * @param y       y-axis value of Point to map
   * @param result  storage for mapped Point
   */
  void MapXY(float x, float y, TPoint* result) const;

  /**
   * Returns Point (x, y) multiplied by Matrix. Given:
   *
   *                | A B C |        | x |
   *       Matrix = | D E F |,  pt = | y |
   *                | G H I |        | 1 |
   *
   * result is computed as:
   *
   *                     |A B C| |x|                               Ax+By+C   Dx+Ey+F
   *       Matrix * pt = |D E F| |y| = |Ax+By+C Dx+Ey+F Gx+Hy+I| = ------- , -------
   *                     |G H I| |1|                               Gx+Hy+I   Gx+Hy+I
   *
   * @param x  x-axis value of Point to map
   * @param y  y-axis value of Point to map
   * @return   mapped Point
   */
  TPoint MapXY(float x, float y) const {
    TPoint result = {};
    this->MapXY(x, y, &result);
    return result;
  }

  /**
   * Sets dst to bounds of src corners mapped by TM33.
   */
  bool MapRect(TRect* dst, const TRect& src) const;

  /**
   * Sets rect to bounds of rect corners mapped by TM33. Returns true if mapped corners are
   * computed rect corners. Returned value is the same as calling rectStaysRect().
   */
  bool MapRect(TRect* rect) const { return MapRect(rect, *rect); }

  /**
   * Returns bounds of src corners mapped by TM33.
   */
  TRect MapRect(const TRect& src) const {
    TRect dst = {};
    MapRect(&dst, src);
    return dst;
  }

  /** Compares a and b; returns true if a and b are numerically equal. Returns true even if sign
   * of zero values are different. Returns false if either TM33 contains NaN, even if the other
   * TM33 also contains NaN.
   */
  friend bool operator==(const TM33& a, const TM33& b);

  /**
   * Compares a and b; returns true if a and b are not numerically equal. Returns false even if
   * sign of zero values are different. Returns true if either TM33 contains NaN, even if the
   * other TM33 also contains NaN.
   */
  friend bool operator!=(const TM33& a, const TM33& b) { return !(a == b); }

  friend TM33 operator*(const TM33& a, const TM33& b) {
    TM33 res;
    res.SetConcat(a, b);
    return res;
  }

  /**
   * Returns true if all elements of the TM33 are finite. Returns false if any
   * element is infinity, or NaN.
   */
  bool IsFinite() const;

  /** Returns true if the matrix contains perspective elements. TM33 form is:

          |       --            --              --          |
          |       --            --              --          |
          | perspective-x  perspective-y  perspective-scale |

      where perspective-x or perspective-y is non-zero, or perspective-scale is
      not one. All other elements may have any value.

      @return  true if TM33 is in most general form
  */
  bool HasPerspective() const {
    if (values[PERSP_0] != 0 || values[PERSP_1] != 0 || values[PERSP_2] != 1) {
      return true;
    }
    return false;
  }

  /** Returns one matrix value. Asserts if index is out of range and SK_DEBUG is
      defined.

      @param index  one of: kMScaleX, kMSkewX, kMTransX, kMSkewY, kMScaleY, kMTransY,
                    kMPersp0, kMPersp1, kMPersp2
      @return       value corresponding to index
  */
  TScalar Get(int index) const {
    assert((unsigned)index < 9);
    return values[index];
  }

  /** Sets TM33 value. Asserts if index is out of range and SK_DEBUG is
        defined. Safer than operator[]; internal cache is always maintained.

        @param index  one of: SCALE_X, SKEW_X, TRANS_X, SKEW_Y, SCALE_Y, TRANS_Y,
                      PERSP_0, PERSP_1, PERSP_2
        @param value  scalar to store in TM33
    */
  TM33& Set(int index, TScalar value) {
    assert((unsigned)index < 9);
    values[index] = value;
    return *this;
  }

  /**
   * TM33 organizes its values in row order. These members correspond to each value in TM33.
   */
  static constexpr int SCALE_X = 0;  //!< horizontal scale factor
  static constexpr int SKEW_X = 1;   //!< horizontal skew factor
  static constexpr int TRANS_X = 2;  //!< horizontal translation
  static constexpr int SKEW_Y = 3;   //!< vertical skew factor
  static constexpr int SCALE_Y = 4;  //!< vertical scale factor
  static constexpr int TRANS_Y = 5;  //!< vertical translation
  static constexpr int PERSP_0 = 6;  //!< input x perspective factor
  static constexpr int PERSP_1 = 7;  //!< input y perspective factor
  static constexpr int PERSP_2 = 8;  //!< perspective bias

  enum TypeMask {
    kIdentity_Mask    = 0,
    kTranslate_Mask   = 0x01,
    kScale_Mask       = 0x02,
    kAffine_Mask      = 0x04,
    kPerspective_Mask = 0x08,
  };

  TypeMask GetType() const {
    // TM33没有在SetScale等方法内更新fTypeMask，故每次调用都要计算一下
    fTypeMask = ComputeTypeMask();
    // only return the public masks
    return (TypeMask)(fTypeMask & 0xF);
  }

  bool IsScaleTranslate() const {
    return !(GetType() & ~(kScale_Mask | kTranslate_Mask));
  }

  bool PreservesAxisAlignment() { return RectStaysRect(); }

  /** Returns true TM33 maps SkRect to another SkRect. If true, TM33 is identity,
    or scales, or rotates a multiple of 90 degrees, or mirrors on axes. In all
    cases, TM33 may also have translation. TM33 form is either:

        | scale-x    0    translate-x |
        |    0    scale-y translate-y |
        |    0       0         1      |

    or

        |    0     rotate-x translate-x |
        | rotate-y    0     translate-y |
        |    0        0          1      |

    for non-zero values of scale-x, scale-y, rotate-x, and rotate-y.

    Also called preservesAxisAlignment(); use the one that provides better inline
    documentation.

    @return  true if TM33 maps one SkRect into another
  */
  bool RectStaysRect() const {
    if (fTypeMask & kUnknownMask) {
      fTypeMask = ComputeTypeMask();
    }
    return (fTypeMask & kRectStaysRectMask) != 0;
  }

  /** Returns true if TM33 at most scales and translates. TM33 may be identity,
  contain only scale elements, only translate elements, or both. TM33 form is:

      | scale-x    0    translate-x |
      |    0    scale-y translate-y |
      |    0       0         1      |

  @return  true if TM33 is identity; or scales, translates, or both
  */
  bool isScaleTranslate() {
    return !(GetType() & ~(kScale_Mask | kTranslate_Mask));
  }

 private:
  /** Set if the matrix will map a rectangle to another rectangle. This
    can be true if the matrix is scale-only, or rotates a multiple of
    90 degrees.

    This bit will be set on identity matrices
*/
  static constexpr int kRectStaysRectMask = 0x10;
  union FloatIntUnion {
    float   fFloat;
    int32_t fSignBitInt;
  };
  static constexpr int kUnknownMask = 0x80;

  static constexpr int kORableMasks = kTranslate_Mask |
                                      kScale_Mask |
                                      kAffine_Mask |
                                      kPerspective_Mask;
  static int32_t Float2Bits(float x) {
    FloatIntUnion data;
    data.fFloat = x;
    return data.fSignBitInt;
  }

  static int32_t SignBitTo2sCompliment(int32_t x) {
    if (x < 0) {
      x &= 0x7FFFFFFF;
      x = -x;
    }
    return x;
  }

  uint8_t ComputeTypeMask() const;
  void SetScaleTranslate(TScalar sx, TScalar sy, TScalar tx, TScalar ty);
  bool InvertNonIdentity(TM33* inverse) const;

  TScalar values[9];
  // make sure fTypeMask is kUnknownMask, ComputeTypeMask() will generate the correct value.
  mutable int32_t fTypeMask = kUnknownMask;
};
}  // namespace tdfcore
