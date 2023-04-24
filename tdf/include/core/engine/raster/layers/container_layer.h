//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <vector>

#include "core/common/id_generator.h"
#include "core/engine/raster/layer_tree.h"
#include "core/engine/raster/layers/layer.h"
#include "core/common/m44.h"

namespace tdfcore {
class LayerTreeBuilder;

/**
 * @brief       单纯的容器Layer
 */
class ContainerLayer : public Layer {
  TDF_REFF_CLASS_META(ContainerLayer)

 public:
  /**
   * @brief     将当前layer以及所有子layer加入LayerTree
   * @param     builder 当前LayerTreeBuilder
   * @param     offset  当前无用，总为(0, 0)
   */
  void AddToLayerTree(std::shared_ptr<LayerTreeBuilder> builder, const TPoint& offset) override;

  /**
   * @brief attach当前layer及所有（递归）后代layer
   * @param child
   */
  void Attach(std::shared_ptr<RenderObject> child) override;
  /**
   * @brief detach当前layer及所有（递归）后代layer
   * @param child
   */
  void Detach() override;

  std::string ToString() const override;

  void VisitChildren(const std::function<void(std::shared_ptr<Layer>& child)>& visitor) override;

  /**
   * @brief     构建出ContainerLayer对应的LayerTree
   * @param     builder
   * @return    当前ContainerLayer对应的LayerTree结构
   */
  std::unique_ptr<LayerTree> BuildLayerTree(const std::shared_ptr<LayerTreeBuilder>& builder);

  /**
   * @brief     在末尾增加子layer
   * @param     child 要增加的子layer
   */
  virtual void Append(std::shared_ptr<Layer> child);

  /**
   * @brief     删除指定的子layer
   * @param     child 要删除的layer
   */
  void RemoveChild(const std::shared_ptr<Layer>& child);

  /**
   * @brief     删除所有子layer
   */
  void RemoveAllChildren();

  const std::vector<std::shared_ptr<Layer>>& GetChildren() { return children_; }

  bool HasChildren() const { return !children_.empty(); }

  const std::vector<std::shared_ptr<Layer>>& GetChildren() const { return children_; }

  /**
   * @brief     将当前layer的变换矩阵左乘至目标矩阵
   * @param     child 当前无用
   * @param     transform 目标矩阵
   */
  virtual void ApplyTransform(const std::shared_ptr<Layer>& child, TM44& transform) {}

  void Preroll(PrerollContext& context, const TM33& matrix) override;

  void Paint(PaintContext& context) const override;

 protected:
  explicit ContainerLayer(uint64_t id = IDGenerator::NextUniqueID());

  /**
   * @brief     将所有孩子节点加入LayerTree
   * @param     builder 当前LayerTreeBuilder
   * @param     offset  当前无用，总为(0, 0)
   */
  void AddChildrenToLayerTree(const std::shared_ptr<LayerTreeBuilder>& builder,
                              const TPoint& offset = TPoint::Make(0, 0));

  void PrerollChildren(PrerollContext& context, const TM33& child_matrix, TRect& child_paint_bounds);
  void PaintChildren(PaintContext& context) const;

  std::vector<std::shared_ptr<Layer>> children_;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ContainerLayer, bases<tdfcore::Layer>)
//  TDF_REFL_MEMBER(children_); //  TODO(kloudwang): 需要实现相应的 TypeProperTy<T>
TDF_REFL_END(tdfcore::ContainerLayer)
