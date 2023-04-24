//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "color_filter.h"
#include "core/common/blend_mode.h"
#include "core/common/impl_getter.h"
#include "core/common/object.h"
#include "core/common/point3.h"
#include "core/engine/graphics/shader.h"

namespace tdfcore {

class TPaint;

class ImageFilter : public Object, public ImplGetter {
 public:
  static std::shared_ptr<ImageFilter> Blur(TScalar sigma_x, TScalar sigma_y,
                                           TileMode tile_mode = TileMode::kDecal,
                                           std::shared_ptr<ImageFilter> input = nullptr,
                                           const TRect& crop_rect = TRect::MakeEmpty());

  static std::shared_ptr<ImageFilter> Erode(TScalar radiusX, TScalar radiusY,
                                            std::shared_ptr<ImageFilter> input,
                                            const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> Dilate(TScalar radiusX, TScalar radiusY,
                                       std::shared_ptr<ImageFilter> input,
                                       const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> MakeFromColorFilter(std::shared_ptr<ColorFilter> cf,
                                                          std::shared_ptr<ImageFilter> input,
                                                          const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> Paint(const TPaint& paint, const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> Blend(BlendMode mode, std::shared_ptr<ImageFilter> background,
                                      std::shared_ptr<ImageFilter> foreground = nullptr,
                                      const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> Offset(TScalar dx, TScalar dy, std::shared_ptr<ImageFilter> input,
                                       const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> Arithmetic(TScalar k1, TScalar k2, TScalar k3, TScalar k4,
                                           bool enforcePMColor, std::shared_ptr<ImageFilter> background,
                                           std::shared_ptr<ImageFilter> foreground,
                                           const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> MakeFromShader(std::shared_ptr<Shader> shader, const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> DisplacementMap(ColorChannel x_channel_selector, ColorChannel y_channel_selector,
                                                      TScalar scale, std::shared_ptr<ImageFilter> displacement,
                                                      std::shared_ptr<ImageFilter> color, const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> DistantLitSpecular(const TPoint3& direction, Color light_color,
                                                   TScalar surface_scale, TScalar ks,
                                                   TScalar shininess, std::shared_ptr<ImageFilter> input,
                                                   const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> PointLitSpecular(const TPoint3& location, Color light_color,
                                                 TScalar surface_scale, TScalar ks,
                                                 TScalar shininess, std::shared_ptr<ImageFilter> input,
                                                 const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> SpotLitSpecular(const TPoint3& location, const TPoint3& target,
                                              TScalar fall_off_exponent, TScalar cut_off_angle,
                                                Color light_color, TScalar surface_scale,
                                                TScalar ks, TScalar shininess,
                                                std::shared_ptr<ImageFilter> input,
                                                const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> DistantLitDiffuse(const TPoint3& direction, Color lightColor,
                                                  TScalar surfaceScale, TScalar kd,
                                                  std::shared_ptr<ImageFilter> input,
                                                  const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> PointLitDiffuse(const TPoint3& location, Color light_color,
                                                      TScalar surface_scale, TScalar kd,
                                                      std::shared_ptr<ImageFilter> input,
                                                const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> SpotLitDiffuse(const TPoint3& location, const TPoint3& target,
                                                     TScalar fall_off_exponent, TScalar cut_off_angle,
                                                     Color light_color, TScalar surface_scale, TScalar kd,
                                                     std::shared_ptr<ImageFilter> input, const TRect& crop_rect = {});

  static std::shared_ptr<ImageFilter> Image(std::shared_ptr<Texture> image, const TRect& src_rect,
                                      const TRect& dst_rect);

  static std::shared_ptr<ImageFilter> Merge(std::shared_ptr<ImageFilter>* const filters, int count,
                                      const TRect& crop_rect = {});

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
