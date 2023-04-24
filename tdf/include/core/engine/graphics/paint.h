//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include <utility>
#include "core/common/blend_mode.h"
#include "core/common/color.h"
#include "core/common/impl_getter.h"
#include "core/common/object.h"
#include "core/common/rect.h"
#include "core/common/scalar.h"
#include "core/engine/graphics/color_filter.h"
#include "core/engine/graphics/image_filter.h"
#include "core/engine/graphics/mask_filter.h"
#include "core/engine/graphics/path.h"
#include "core/engine/graphics/path_effect.h"
#include "core/engine/graphics/shader.h"

namespace tdfcore {
class PaintImpl;

class TPaint : public Object, public ImplGetter {
 public:
  ~TPaint() override = default;
  TPaint();
  TPaint(const TPaint& paint);
  TPaint(TPaint&& paint);
  explicit TPaint(const Color& color);
  TPaint& operator=(const TPaint& paint);

  enum class Style {
    kFill_Style,    //!< set to fill geometry
    kStroke_Style,  //!< set to stroke geometry
    kStrokeAndFill_Style,
  };

  enum class Cap {
    kButt_Cap,    //!< no stroke extension
    kRound_Cap,   //!< adds circle
    kSquare_Cap,  //!< adds square
  };

  enum class Join {
    kMiter_Join,  //!< extends to miter limit
    kRound_Join,  //!< adds circle
    kBevel_Join,  //!< connects outside edges
  };

  void SetColor(uint32_t argb);

  void SetColor(Color color);

  Color GetColor() const;

  void SetStyle(Style style);

  Style GetStyle() const;

  void SetStroke(bool is_stroke);

  void SetStrokeWidth(TScalar width);

  TScalar GetStrokeWidth() const;

  void SetStrokeCap(Cap cap);

  Cap GetStrokeCap() const;

  void SetStrokeJoin(Join join);

  Join GetStrokeJoin() const;

  void SetAntiAlias(bool aa);

  bool IsAntiAlias() const;

  void SetBlendMode(BlendMode mode);

  BlendMode GetBlendMode() const;

  void SetAlpha(Alpha a);

  uint8_t GetAlpha() const;

  void SetShader(const std::shared_ptr<Shader>& shader);

  std::shared_ptr<Shader> GetShader() const;

  void SetMaskFilter(const std::shared_ptr<MaskFilter>& mask_filter);

  std::shared_ptr<MaskFilter> GetMaskFilter() const;

  void SetColorFilter(const std::shared_ptr<ColorFilter>& color_filter);

  std::shared_ptr<ColorFilter> GetColorFilter() const;

  void SetImageFilter(const std::shared_ptr<ImageFilter>& image_filter);

  std::shared_ptr<ImageFilter> GetImageFilter() const;

  bool NothingToDraw() const;

  bool CanComputeFastBounds() const;

  TRect ComputeFastBounds(TRect& orig, TRect* storage) const;

  bool GetFillPath(const Path& src, Path* dst, const TRect* cull_rect, TScalar res_scale = 1) const;

  bool GetFillPath(const Path& src, Path* dst, const TRect* cull_rect, const TM33& ctm) const;

  void SetPathEffect(const std::shared_ptr<PathEffect>& path_effect);

  void SetStrokeMiter(TScalar miter);

  TScalar GetStrokeMiter() const;

  std::shared_ptr<PathEffect> GetPathEffect() const;

  void CleanPathEffect();

  void Reset();

  friend bool operator==(const TPaint& a, const TPaint& b);

  friend bool operator!=(const TPaint& a, const TPaint& b) { return !(a == b); }

 protected:
  std::any GetConst() const override;
  std::any Get() override;
  std::shared_ptr<PaintImpl> impl_;
};

}  // namespace tdfcore
