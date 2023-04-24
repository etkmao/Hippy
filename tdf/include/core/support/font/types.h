//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <stdint.h>
#include "core/common/scalar.h"

namespace tdfcore {

typedef uint16_t GlyphID;
typedef uint32_t TTypefaceID;
typedef uint32_t TFontTableTag;
typedef int32_t Unichar;
typedef uint32_t FourByteTag;
static inline constexpr FourByteTag SetFourByteTag(char a, char b, char c, char d) {
  return (((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)c << 8) | (uint32_t)d);
}
enum class TTextEncoding {
  kUTF8,     //!< uses bytes to represent UTF-8 or ASCII
  kUTF16,    //!< uses two byte words to represent most of Unicode
  kUTF32,    //!< uses four byte words to represent all of Unicode
  kGlyphID,  //!< uses two byte words to represent glyph indices
};

enum class TFontHinting {
  kNone,    //!< glyph outlines unchanged
  kSlight,  //!< minimal modification to improve constrast
  kNormal,  //!< glyph outlines modified to improve constrast
  kFull,    //!< modifies glyph outlines for maximum constrast
};

enum class Edging {
  kAlias,
  kAntiAlias,
  kSubpixelAntiAlias,
};

struct TFontMetrics {
  bool operator==(const TFontMetrics& that) const {
    return this->flags == that.flags && this->top == that.top && this->ascent == that.ascent &&
           this->descent == that.descent && this->bottom == that.bottom && this->leading == that.leading &&
           this->avg_char_width == that.avg_char_width && this->max_char_width == that.max_char_width &&
           this->x_min == that.x_min && this->x_max == that.x_max && this->x_height == that.x_height &&
           this->cap_height == that.cap_height && this->underline_thickness == that.underline_thickness &&
           this->underline_position == that.underline_position &&
           this->strikeout_thickness == that.strikeout_thickness && this->strikeout_position == that.strikeout_position;
  }

  enum FontMetricsFlags {
    kUnderlineThicknessIsValid_Flag = 1 << 0,  //!< set if fUnderlineThickness is valid
    kUnderlinePositionIsValid_Flag = 1 << 1,   //!< set if fUnderlinePosition is valid
    kStrikeoutThicknessIsValid_Flag = 1 << 2,  //!< set if fStrikeoutThickness is valid
    kStrikeoutPositionIsValid_Flag = 1 << 3,   //!< set if fStrikeoutPosition is valid
    kBoundsInvalid_Flag = 1 << 4,              //!< set if fTop, fBottom, fXMin, fXMax invalid
  };

  uint32_t flags;  //!< FontMetricsFlags indicating which metrics are valid
  TScalar top;     //!< greatest extent above origin of any glyph bounding box, typically negative; deprecated with
  //!< variable fonts
  TScalar ascent;   //!< distance to reserve above baseline, typically negative
  TScalar descent;  //!< distance to reserve below baseline, typically positive
  TScalar bottom;   //!< greatest extent below origin of any glyph bounding box, typically positive; deprecated with
  //!< variable fonts
  TScalar leading;         //!< distance to add between lines, typically positive or zero
  TScalar avg_char_width;  //!< average character width, zero if unknown
  TScalar max_char_width;  //!< maximum character width, zero if unknown
  TScalar x_min;  //!< greatest extent to left of origin of any glyph bounding box, typically negative; deprecated with
  //!< variable fonts
  TScalar x_max;  //!< greatest extent to right of origin of any glyph bounding box, typically positive; deprecated with
  //!< variable fonts
  TScalar x_height;             //!< height of lower-case 'x', zero if unknown, typically negative
  TScalar cap_height;           //!< height of an upper-case letter, zero if unknown, typically negative
  TScalar underline_thickness;  //!< underline thickness
  TScalar underline_position;   //!< distance from baseline to fTop of stroke, typically positive
  TScalar strikeout_thickness;  //!< strikeout thickness
  TScalar strikeout_position;   //!< distance from baseline to fBottom of stroke, typically negative

  bool HasUnderlineThickness(TScalar& thickness) const {
    if (flags & kUnderlineThicknessIsValid_Flag) {
      thickness = underline_thickness;
      return true;
    }
    return false;
  }

  /** Returns true if SkFontMetrics has a valid underline position, and sets
   position to that value. If the underline position is not valid,
   return false, and ignore position.

   @param position  storage for underline position
   @return          true if font specifies underline position
   */
  bool HasUnderlinePosition(TScalar& position) const {
    if (flags & kUnderlinePositionIsValid_Flag) {
      position = underline_position;
      return true;
    }
    return false;
  }

  /** Returns true if SkFontMetrics has a valid strikeout thickness, and sets
   thickness to that value. If the underline thickness is not valid,
   return false, and ignore thickness.

   @param thickness  storage for strikeout width
   @return           true if font specifies strikeout width
   */
  bool HasStrikeoutThickness(TScalar& thickness) const {
    if (flags & kStrikeoutThicknessIsValid_Flag) {
      thickness = strikeout_thickness;
      return true;
    }
    return false;
  }

  /** Returns true if SkFontMetrics has a valid strikeout position, and sets
   position to that value. If the underline position is not valid,
   return false, and ignore position.

   @param position  storage for strikeout position
   @return          true if font specifies strikeout position
   */
  bool HasStrikeoutPosition(TScalar& position) const {
    if (flags & kStrikeoutPositionIsValid_Flag) {
      position = strikeout_position;
      return true;
    }
    return false;
  }

  /** Returns true if SkFontMetrics has a valid fTop, fBottom, fXMin, and fXMax.
   If the bounds are not valid, return false.

   @return        true if font specifies maximum glyph bounds
   */
  bool HasBounds() const { return !(flags & kBoundsInvalid_Flag); }
};

}  // namespace tdfcore
