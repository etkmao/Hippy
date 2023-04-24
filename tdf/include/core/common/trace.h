//
//  Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/graphics/texture.h"

namespace tdfcore {
class Trace {
 public:
  static void TraceTexture(const std::shared_ptr<Device> &device, const std::shared_ptr<Texture> &texture);

  static void TraceZoneOfFunction(const char* func_name);
};
}

