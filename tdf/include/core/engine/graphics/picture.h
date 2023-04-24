//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include <variant>
#include "core/common/object.h"
#include "core/common/rect.h"
#include "core/common/rrect.h"
#include "core/common/m44.h"
#include "core/common/color.h"
#include "core/engine/graphics/texture.h"
#include "core/engine/graphics/path.h"
#include "core/engine/graphics/paint.h"
#include "core/engine/graphics/save_layer_rec.h"
#include "core/support/text/text_blob.h"

namespace tdfcore {
class Canvas;

struct PictureDraw {
  Canvas* canvas;
  TM44 initial_ctm;
};

class Picture : public Object {
 public:
  ~Picture() override = default;

  static std::shared_ptr<Picture> MakePlaceholder(TRect cull_rect);

  static std::shared_ptr<Texture> MakeTexture(const std::shared_ptr<Device>& device,
                                              const std::shared_ptr<Picture> picture, const ISize& dimensions,
                                              const TM33& matrix, const TPaint& paint);

  TRect cullRect() { return cull_rect_; }

  uint32_t uniqueID() const { return unique_id_; }

  int approximateOpCount(bool nested = false) const { return static_cast<int>(drawcalls_.size()); }

  virtual void playback(Canvas *canvas) const;

 protected:
  explicit Picture(std::vector<std::function<void(PictureDraw&)>>& drawcalls, TRect& cull_rect);

  std::vector<std::function<void(PictureDraw&)>> drawcalls_;

  uint32_t unique_id_ = 0;
  TRect cull_rect_;

 private:
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
