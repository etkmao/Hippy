//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/impl_getter.h"
#include "core/common/object.h"
#include "core/common/scalar.h"
#include "core/engine/graphics/path.h"

namespace tdfcore {
class PathEffect : public Object, public ImplGetter {
 public:
  static std::shared_ptr<PathEffect> MakeCompose(std::shared_ptr<PathEffect> outer, std::shared_ptr<PathEffect> inner);

  FRIEND_OF_TDF_ALLOC;
};

class DashPathEffect {
 public:
  static std::shared_ptr<PathEffect> Make(const TScalar intervals[], int count, TScalar phase);
};

class Path1DPathEffect {
 public:
  enum Style {
    kTranslate_Style,  // translate the shape to each position
    kRotate_Style,     // rotate the shape about its center
    kMorph_Style,      // transform each point, and turn lines into curves
  };

  static std::shared_ptr<PathEffect> Make(const Path& path, TScalar advance, TScalar phase, Style);
};

}  // namespace tdfcore
