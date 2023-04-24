//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <unordered_map>
#include <optional>
#include "core/platform/common/view/mutators_stack.h"

namespace tdfcore {
class Canvas;
class PictureRecorder;
class Surface;

/**
 * @brief 动态化框架画布
 * 提供了一个主画布、和多个Overlay画布
 */
class Board : public Object {
 public:
  static std::shared_ptr<Board> Make(const std::shared_ptr<Surface> &main_surface,
                                     const std::unordered_map<int64_t, std::shared_ptr<Surface>> &overlay_surfaces);
  static std::shared_ptr<Board> MakeNull(int64_t width, int64_t height);

  /**
   * @brief 获取主Canvas
   * `is_canvas_from_sk_picture_recorder`为true 返回SkSurface创建的canvas.
   * `is_canvas_from_sk_picture_recorder`为false 返回SkPictureRecorder创建的canvas.
   * @return 主Canvas
   */
  virtual Canvas *GetMainCanvas() const = 0;

  /**
   * @brief 获取所有的Overlay Canvas
   * `is_canvas_from_sk_picture_recorder`为true 返回SkSurface创建的canvas
   * `is_canvas_from_sk_picture_recorder`为false 返回SkPictureRecorder创建的canvas
   * @return Overlay Canvas
   */
  virtual const std::unordered_map<int64_t, Canvas *> &GetOverlayCanvases() const = 0;

  /**
   * @brief 执行绘制指令，需要根据是否是通过PictureRecorder收集的绘制指令
   * 如果是通过PictureRecorder获取的Canvas来进行Rasterizer，需要回放PictureRecorder收集的绘制指令
   * @param id
   */
  virtual void Flush(std::optional<int64_t> id) = 0;

  /**
   * @brief 收集Embedded对应的MutatorsStack信息
   * @param id
   * @param mutators_stack
   */
  virtual void SetEmbeddedMutatorStack(int64_t id, const MutatorsStack &mutators_stack) = 0;

  /**
   * @brief 获取收集到的EmbeddedMutatorStacks信息
   * @return
   */
  virtual std::unordered_map<int64_t, MutatorsStack> GetEmbeddedMutatorsStacks() const = 0;

  virtual ISize GetSize() = 0;
};
}  // namespace tdfcore
