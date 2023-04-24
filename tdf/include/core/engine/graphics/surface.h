//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include "core/common/object.h"
#include "core/engine/graphics/texture.h"

namespace tgfx {
class Texture;
}

namespace tdfcore {
class Canvas;
class TPaint;
class Surface : public Object {
 public:
  static std::shared_ptr<Surface> MakeNull(int width, int height);

  static std::shared_ptr<Surface> Make(const std::shared_ptr<Device> &deivce, int width, int height);

  static std::shared_ptr<Surface> Make(const std::shared_ptr<Device> &deivce, const std::shared_ptr<Texture> &texture);

  virtual ISize GetSize() const = 0;

  virtual Canvas *GetCanvas() = 0;

  virtual std::shared_ptr<Texture> MakeSnapshot(const std::shared_ptr<Device> &device) = 0;
};

}  // namespace tdfcore
