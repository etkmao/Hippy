//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/render_object.h"
#include "core/support/gesture/recognizer/gesture_recognizer.h"

namespace tdfcore {

using GestureRecognizers = std::vector<std::shared_ptr<GestureRecognizer>>;

class GestureHandler {
 public:
  GestureHandler() = default;

  /**
   * 往 render_object 添加 recognizer
   * @param render_object 需要添加手势的 render_object
   * @param recognizer 需要添加的手势
   */
  static void AddGesture(const std::shared_ptr<RenderObject> &render_object,
                         const std::shared_ptr<GestureRecognizer> &recognizer);

  /**
   * 将 render_object 的 recognizer 移除
   * @param render_object 需要移除手势的 render_object
   * @param recognizer 需要移除的手势
   */
  static void RemoveGesture(const std::shared_ptr<RenderObject> &render_object,
                            const std::shared_ptr<GestureRecognizer> &recognizer);

  /**
   * 获取 render_object 的手势集合
   * @param render_object 需要获取手势集合的 render_object
   * @return 手势集合
   */
  static GestureRecognizers GetGestureRecognizers(const std::shared_ptr<RenderObject> &render_object);
};
}  // namespace tdfcore
