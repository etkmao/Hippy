//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include "core/common/impl_getter.h"
#include "core/common/object.h"
#include "core/common/scalar.h"

namespace tdfcore {

enum BlurStyle : int {
  kNormalBlurStyle,  //!< fuzzy inside and outside
  kSolidBlurStyle,   //!< solid inside, fuzzy outside
  kOuterBlurStyle,   //!< nothing inside, fuzzy outside
  kInnerBlurStyle,   //!< fuzzy inside, nothing outside
  kLastEnumBlurStyle = kInnerBlurStyle,
};

class MaskFilter : public Object, public ImplGetter {
 public:
  /**
   * @brief Create a blur maskfilter.
   * @param style The BlurStyle to use
   * @param sigma Standard deviation of the Gaussian blur to apply. Must be > 0.
   * @param respect_ctm if true the blur's sigma is modified by the CTM.
   */
  static std::shared_ptr<MaskFilter> MakeBlur(BlurStyle style, TScalar sigma, bool respect_ctm = true);

 private:
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
