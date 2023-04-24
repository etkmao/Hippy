//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include "core/common/rect.h"
#include "core/engine/graphics/image_filter.h"
#include "core/engine/graphics/paint.h"

namespace tdfcore {

typedef uint32_t SaveLayerFlags;

struct SaveLayerRec {
  SaveLayerRec() {}

  SaveLayerRec(const TRect* bounds, const TPaint* paint, SaveLayerFlags saveLayerFlags = 0)
      : fBounds(bounds), fPaint(paint), fSaveLayerFlags(saveLayerFlags) {}

  SaveLayerRec(const TRect* bounds, const TPaint* paint, const ImageFilter* backdrop, SaveLayerFlags saveLayerFlags)
      : fBounds(bounds), fPaint(paint), fBackdrop(backdrop), fSaveLayerFlags(saveLayerFlags) {}

  const TRect* fBounds = nullptr;
  const TPaint* fPaint = nullptr;
  std::shared_ptr<const ImageFilter> fBackdrop = nullptr;
  SaveLayerFlags fSaveLayerFlags = 0;
};

}  // namespace tdfcore
