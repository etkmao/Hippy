//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include "core/common/color.h"
#include "core/common/m44.h"
#include "core/common/rect.h"

namespace tdfcore {
class Canvas;

void DrawCheckerboard(Canvas *canvas, Color c1, Color c2, int size);

void DrawCheckerboard(Canvas *canvas, const TRect &rect);

TRect InverseTransformRect(TM44 transform, TRect rect);

}  // namespace tdfcore
