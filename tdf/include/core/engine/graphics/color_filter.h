//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/common/impl_getter.h"
#include "core/common/object.h"
#include "core/common/color.h"
#include "core/common/blend_mode.h"

namespace tdfcore {

class ColorFilter : public Object, public ImplGetter {
 public:
  static std::shared_ptr<ColorFilter> Matrix(const std::array<float, 20>& rowMajor);

  static std::shared_ptr<ColorFilter> LinearToSRGBGamma();

  static std::shared_ptr<ColorFilter> SRGBToLinearGamma();

  static std::shared_ptr<ColorFilter> LumaColorFilter();

  static std::shared_ptr<ColorFilter> Compose(std::shared_ptr<ColorFilter>, std::shared_ptr<ColorFilter>);

  bool AsAColorMode(Color* color, BlendMode* mode) const;

 private:
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
