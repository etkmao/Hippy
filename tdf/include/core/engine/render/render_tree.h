//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/render/render_object.h"
#include "core/engine/render/render_property.h"
#include "core/platform/common/viewport_metrics.h"

namespace tdfcore {
/**
 * @brief RenderTree
 */
class RenderTree : public Object {
 public:
  /**
   * @brief 设置视图参数
   * @param viewport_metrics 设置视图参数
   */
  void SetViewportMetrics(const ViewportMetrics &viewport_metrics);

  /**
   * @brief 获取视图参数
   * @return 视图参数
   */
  const ViewportMetrics &GetViewportMetrics();

  /**
   * @brief 装载
   */
  void Attach(const std::shared_ptr<RenderContext> &render_context);

  /**
   * @brief 获取根节点
   */
  constexpr const std::shared_ptr<RenderObject> &GetRootObject() const { return root_object_; }

 protected:
  explicit RenderTree(const TM33 &root_transform = TM33());

  /**
   * @brief 设置对根节点的初始变换参数，用于设备的逻辑坐标系向绘制的像素坐标系的转换
   * @param transform transform
   */
  void SetRootTransform(const TM33 &transform);

 private:
  std::shared_ptr<RenderObject> root_object_;
  TM33 root_transform_ = TM33();
  ViewportMetrics viewport_metrics_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
