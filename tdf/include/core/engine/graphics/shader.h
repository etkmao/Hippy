//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/common/color.h"
#include "core/common/impl_getter.h"
#include "core/common/m33.h"
#include "core/common/object.h"
#include "core/common/point.h"
#include "core/common/scalar.h"

namespace tdfcore {
class Texture;
class Bitmap;

enum class TileMode {
  kClamp,
  kRepeat,
  kMirror,
  kDecal,
};

enum class GradientType {
  kNoneGradientType,
  kColorGradientType,
  kLinearGradientType,
  kRadialGradientType,
  kSweepGradientType,
  kConicalGradientType,
  kLastGradientType = kConicalGradientType,
};

struct GradientInfo {
  int color_count;         //!< In-out parameter, specifies passed size
  Color* colors;           //!< The colors in the gradient.
  TScalar* color_offsets;  //!< The unit offset for color transitions.
  TPoint point[2];         //!< Type specific, see above.
  TScalar radius[2];       //!< Type specific, see above.
  TileMode tile_mode;
  uint32_t gradient_flags;  //!< see SkGradientShader::Flags
};

class Shader : public Object, public ImplGetter {
 public:
  static std::shared_ptr<Shader> MakeLinearGradient(const TPoint pts[2], const Color colors[], const TScalar pos[],
                                                    int count, TileMode mode, uint32_t flags, const TM33* localMatrix);

  static std::shared_ptr<Shader> MakeRadialGradient(const TPoint& center, TScalar radius, const Color colors[],
                                                    const TScalar pos[], int count, TileMode mode, uint32_t flags,
                                                    const TM33* localMatrix);

  static std::shared_ptr<Shader> MakeSweepGradient(TScalar cx, TScalar cy, const Color colors[], const TScalar pos[],
                                                   int count, TileMode mode, TScalar startAngle, TScalar endAngle,
                                                   uint32_t flags, const TM33* localMatrix);

  static std::shared_ptr<Shader> MakeTwoPointConicalGradient(const TPoint& start, TScalar startRadius,
                                                             const TPoint& end, TScalar endRadius, const Color colors[],
                                                             const TScalar pos[], int count, TileMode mode,
                                                             uint32_t flags, const TM33* localMatrix);

  static std::shared_ptr<Shader> CreateCheckerboardShader(Color c1, Color c2, int size);

  static std::shared_ptr<Shader> MakeFromTexture(const std::shared_ptr<Texture>& texture, TileMode tmx, TileMode tmy,
                                                 const TM33* local_matrix = nullptr);

  static std::shared_ptr<Shader> MakeFromBitmap(const std::shared_ptr<Bitmap>& bitmap, TileMode tmx, TileMode tmy,
                                                const TM33* local_matrix = nullptr);

  static std::shared_ptr<Shader> MakeFromColor(const Color & color);

  static std::shared_ptr<Shader> MakeFractalNoise(TScalar base_frequency_x, TScalar base_frequency_y,
                                                  int num_octaves, TScalar seed,
                                                  const ISize* tile_size = nullptr);

  static std::shared_ptr<Shader> MakeTurbulence(TScalar base_frequency_x, TScalar base_frequency_y,
                                                    int num_octaves, TScalar seed,
                                                    const ISize* tile_size = nullptr);

  std::shared_ptr<Texture> IsAImage(TM33* local_matrix, TileMode xy[2]) const;

  GradientType AsAGradient(GradientInfo* info) const;

  TM33 GetLocalMatrix() const;

 private:
  void SetTexture(const std::shared_ptr<Texture>& texture);
  std::shared_ptr<Texture> texture_ = nullptr;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
