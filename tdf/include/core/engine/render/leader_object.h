//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/engine/raster/layers/layer_link.h"
#include "core/engine/render/render_object.h"

namespace tdfcore {

/**
 * @brief 位置跟随系统中的引导节点，是被跟随的对象
 */
class LeaderObject : public RenderObject {
  TDF_REFF_CLASS_META(LeaderObject)
 public:
  /**
   * @brief 返回LayerLink信息
   */
  std::shared_ptr<LayerLink> GetLink() const;

  /**
   * @brief 设置LayerLink信息
   */
  void SetLink(std::shared_ptr<LayerLink> link);

 protected:
  explicit LeaderObject(std::shared_ptr<LayerLink> link);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

  bool AlwaysNeedsCompositing() override { return true; }

 private:
  std::shared_ptr<LayerLink> link_ = nullptr;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::LeaderObject, bases<tdfcore::RenderObject>)
  TDF_REFL_MEMBER(link_);
TDF_REFL_END(tdfcore::LeaderObject)
