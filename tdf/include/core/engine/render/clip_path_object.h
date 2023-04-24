//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/clip_object.h"
#include "core/engine/graphics/path.h"
#include "core/engine/render/render_property.h"

namespace tdfcore {

/**
 * @brief 任意形状剪裁
 * @details 通过路径来描述剪裁区域的形状，子节点将被应用该剪裁效果
 */
class ClipPathObject : public ClipObject {
  TDF_REFF_CLASS_META(ClipPathObject)

 public:
  /**
   * @brief 通过路径来设置剪裁区域
   * @param path 剪裁路径
   */
  void SetClipPath(const std::shared_ptr<Path> &clip_path);

  /**
   * @brief 获取裁剪路径
   */
  std::shared_ptr<Path> GetClipPath();

 protected:
  TRect GetClipBounds() override;
  explicit ClipPathObject(const std::shared_ptr<Path> &clip_path);

  std::shared_ptr<Layer> Clip(PaintingContext &context, TPoint offset,
                              PaintingContext::PaintingContextCallback painter) override;

  bool HitTestClip(const TPoint &position) const override;

  void OnDebugPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  std::shared_ptr<Path> clip_path_ = nullptr;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ClipPathObject, bases<tdfcore::ClipObject>)
TDF_REFL_END(tdfcore::ClipPathObject)
