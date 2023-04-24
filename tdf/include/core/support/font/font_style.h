//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

namespace tdfcore {

class TFontStyle {
 public:
  enum Weight {
    kInvisible_Weight = 0,
    kThin_Weight = 100,
    kExtraLight_Weight = 200,
    kLight_Weight = 300,
    kNormal_Weight = 400,
    kMedium_Weight = 500,
    kSemiBold_Weight = 600,
    kBold_Weight = 700,
    kExtraBold_Weight = 800,
    kBlack_Weight = 900,
    kExtraBlack_Weight = 1000,
  };

  enum Width {
    kUltraCondensed_Width = 1,
    kExtraCondensed_Width = 2,
    kCondensed_Width = 3,
    kSemiCondensed_Width = 4,
    kNormal_Width = 5,
    kSemiExpanded_Width = 6,
    kExpanded_Width = 7,
    kExtraExpanded_Width = 8,
    kUltraExpanded_Width = 9,
  };

  enum Slant {
    kUpright_Slant,
    kItalic_Slant,
    kOblique_Slant,
  };

  static constexpr int TPin(int x, int lo, int hi) { return std::max(lo, std::min(x, hi)); }

  constexpr TFontStyle(int weight, int width, Slant slant)
      : value_((TPin(weight, kInvisible_Weight, kExtraBlack_Weight)) +
               (TPin(width, kUltraCondensed_Width, kUltraExpanded_Width) << 16) +
               (TPin(slant, kUpright_Slant, kOblique_Slant) << 24)) {}

  constexpr TFontStyle() : TFontStyle{kNormal_Weight, kNormal_Width, kUpright_Slant} {}

  bool operator==(const TFontStyle& rhs) const { return value_ == rhs.value_; }

  int weight() const { return value_ & 0xFFFF; }
  int width() const { return (value_ >> 16) & 0xFF; }
  Slant slant() const { return (Slant)((value_ >> 24) & 0xFF); }

  static constexpr TFontStyle Normal() { return {kNormal_Weight, kNormal_Width, kUpright_Slant}; }
  static constexpr TFontStyle Bold() { return {kBold_Weight, kNormal_Width, kUpright_Slant}; }
  static constexpr TFontStyle Italic() { return {kNormal_Weight, kNormal_Width, kItalic_Slant}; }
  static constexpr TFontStyle BoldItalic() { return {kBold_Weight, kNormal_Width, kItalic_Slant}; }

 private:
  int32_t value_;
};

}  // namespace tdfcore
