//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include "core/common/data.h"
#include "core/common/object.h"
#include "core/common/size.h"
#include "core/engine/graphics/device.h"

namespace tdfcore {

class Texture : public Object {
 public:
  virtual int Width() const = 0;

  virtual int Height() const = 0;

  ISize Dimensions() const;

  static std::shared_ptr<Texture> Make(std::shared_ptr<Device> device, int width, int height);
};

}  // namespace tdfcore
