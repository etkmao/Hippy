//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <vector>
#include <atomic>
#include "core/common/object.h"

namespace tdfcore {
class RenderObject;
class RenderPipeline;

class RenderContext : public Object, public std::enable_shared_from_this<RenderContext> {
 public:
  ~RenderContext() override = default;
  /**
   * @brief 添加被标脏的RenderObject
   * @param 被标脏的RenderObject
   */
  void AddDirtyObject(std::shared_ptr<RenderObject> obj);
  void AddCompositingUpdateObject(std::shared_ptr<RenderObject> obj);
  void FlushCompositingFlag();
  void FlushPaint();
  void Clear();

  static std::shared_ptr<RenderContext> GetCurrent();

  /**
   * @brief 判断页面中是否有脏数据
   * @return 是否有脏数据
   */
  bool IsDirty() const {
    return needs_paint_.load() || needs_compositing_update_.load();
  }

  /**
   * @brief 判断是否需要绘制
   * @return 是否需要绘制
   */
  bool NeedsPaint() const {
    return needs_paint_.load();
  }

  /**
   * @brief 获取渲染树根节点
   * @return 渲染树根节点
   */
  std::shared_ptr<RenderObject> GetRootNode() const;

 private:
  void SetCurrent();

  std::vector<std::shared_ptr<RenderObject>> dirty_objects_;
  std::vector<std::shared_ptr<RenderObject>> compositing_update_objects_;
  std::shared_ptr<RenderObject> root_node_;

  std::atomic<bool> needs_paint_ = false;
  std::atomic<bool> needs_compositing_update_ = false;

  friend class RenderPipeline;
};
}  // namespace tdfcore
