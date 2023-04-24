//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include "core/common/object.h"
#include "core/engine/graphics/canvas.h"

class SkPictureRecorder;

namespace tdfcore {

class PictureRecorder : public Object {
 public:
  PictureRecorder();
  ~PictureRecorder() override;

  Canvas *beginRecording(const TRect& bounds);

  Canvas *beginRecording(TScalar width, TScalar height) {
    return this->beginRecording(TRect::MakeWH(width, height));
  }

  std::shared_ptr<Picture> finishRecordingAsPicture();

 private:
  std::shared_ptr<Canvas> canvas_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
