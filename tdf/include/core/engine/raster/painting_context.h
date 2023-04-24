//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <functional>
#include <memory>

#include "core/common/m33.h"
#include "core/common/memory.h"
#include "core/common/point.h"
#include "core/common/rect.h"
#include "core/common/rrect.h"
#include "core/engine/graphics/canvas.h"
#include "core/engine/graphics/color_filter.h"
#include "core/engine/graphics/path.h"
#include "core/engine/graphics/picture_recorder.h"
#include "core/engine/render/custom_painter.h"

namespace tdfcore {

class RenderObject;
class Layer;
class ClipRectLayer;
class ClipPathLayer;
class ColorFilterLayer;
class TransformLayer;
class OpacityLayer;
class ContainerLayer;
class PictureLayer;

/**
 * @brief       RenderTree绘制上下文
 */
class PaintingContext {
 public:
  using PaintingContextCallback = std::function<void(PaintingContext& context, TPoint offset)>;

  static void RepaintCompositedChild(const std::shared_ptr<RenderObject>& child);

  void PaintChild(const std::shared_ptr<RenderObject>& child, TPoint offset = TPoint::Make(0, 0));
  std::shared_ptr<Layer> PushClipRect(bool needs_compositing, TPoint offset, TRect clip_rect,
                                      const PaintingContextCallback& painter, const std::shared_ptr<Layer>& old_layer,
                                      bool is_anti_alias = true);

  std::shared_ptr<Layer> PushClipRRect(bool needs_compositing, TPoint offset, TRect bounds, RRect clip_rrect,
                                       const PaintingContextCallback& painter, const std::shared_ptr<Layer>& old_layer,
                                       bool is_anti_alias = true);

  std::shared_ptr<Layer> PushClipPath(bool needs_compositing, TPoint offset, TRect bounds, const Path& clip_path,
                                      const PaintingContextCallback& painter, const std::shared_ptr<Layer>& old_layer,
                                      bool is_anti_alias = true);
  std::shared_ptr<Layer> PushColorFilter(TPoint offset, const std::shared_ptr<ColorFilter>& color_filter,
                                         const PaintingContextCallback& painter,
                                         const std::shared_ptr<Layer>& old_layer);
  std::shared_ptr<Layer> PushTransform(bool needs_compositing, TPoint offset, TM33 transform,
                                       const PaintingContextCallback& painter, const std::shared_ptr<Layer>& old_layer);
  std::shared_ptr<Layer> PushOpacity(TPoint offset, Alpha alpha, const PaintingContextCallback& painter,
                                     const std::shared_ptr<Layer>& old_layer);
  void AddLayer(const std::shared_ptr<Layer>& layer);
  void PushLayer(const std::shared_ptr<ContainerLayer>& child_layer, const PaintingContextCallback& painter,
                 TPoint offset, TRect child_paint_bounds = TRect::MakeEmpty());

  virtual Canvas* GetCanvas();

 protected:
  PaintingContext(std::shared_ptr<ContainerLayer> layer, TRect estimated_bounds);
  void AppendLayer(const std::shared_ptr<Layer>& layer);

 private:
  bool IsRecording() const { return canvas_ != nullptr; }
  void StartRecording();
  void StopRecordingIfNeeded();
  void CompositeChild(const std::shared_ptr<RenderObject>& child, TPoint offset);

  std::shared_ptr<PictureLayer> current_layer_ = nullptr;
  std::shared_ptr<ContainerLayer> container_layer_ = nullptr;

  TRect bounds_ = TRect::MakeLTRB(TScalarNegativeInfinity, TScalarNegativeInfinity, TScalarInfinity, TScalarInfinity);
  Canvas* canvas_ = nullptr;
  std::unique_ptr<PictureRecorder> picture_recorder_ = nullptr;

  bool is_debug_paint_layer_borders_enabled_ = false;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
