//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/m33.h"
#include "core/common/m44.h"
#include "core/common/macro.h"

namespace tdfcore {

// todo: 基础能力，放到别的文件夹
/// Decomposes a given non-degenerate transformation matrix into a sequence of
/// operations that produced it. The validity of the decomposition must always
/// be checked before attempting to access any of the decomposed elements.
class MatrixDecomposition {
 public:
  explicit MatrixDecomposition(const TM33& matrix);

  explicit MatrixDecomposition(TM44 matrix);

  ~MatrixDecomposition();

  bool IsValid() const;

  const TV3& translation() const { return translation_; }

  const TV3& scale() const { return scale_; }

  const TV3& shear() const { return shear_; }

  const TV4& perspective() const { return perspective_; }

  const TV4& rotation() const { return rotation_; }

 private:
  bool valid_;
  TV3 translation_;
  TV3 scale_;
  TV3 shear_;
  TV4 perspective_;
  TV4 rotation_;
};

}  // namespace tdfcore
