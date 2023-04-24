//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/engine/render/render_object.h"

namespace tdfcore {
class LayerLink;
/**
 * @brief 变换跟随修饰渲染对象
 */
class FollowerObject : public RenderObject {
  TDF_REFF_CLASS_META(FollowerObject)

 public:
  /**
   * @brief 返回LayerLink对象
   * @return
   */
  constexpr const std::shared_ptr<LayerLink> &GetLink() const { return link_; }

  /**
   * @brief 设置LayerLink对象
   */
  void SetLink(const std::shared_ptr<LayerLink> &link);

  /**
   * @brief 返回跟随时额外的偏移
   */
  constexpr const TPoint &GetFollowOffset() const { return follow_offset_; }

  /**
   * @brief 设置offset
   */
  void SetFollowOffset(const TPoint &offset);

  /**
   * @brief 返回当切断RenderLeaderLayer关联时是否显示
   * @return
   */
  bool ShowWhenUnlinked() const;

  /**
   * @brief 设置当切断RenderLeaderLayer关联时是否显示
   * @param show_when_unlinked
   */
  void SetShowWhenUnlinked(bool show_when_unlinked);

  /**
   * @brief 返回当前变换矩阵
   */
  TM33 GetCurrentMatrix() const;

  /**
   * @brief 碰撞测试，把当前position位置中下所有 HitTestTarget 添加到 HitTestResult 中去。
   *
   * @param position 碰撞的位置
   * @param transform 相对于根节点的转变
   * @return 碰撞的结果集合
   */
  std::shared_ptr<HitTestResult> HitTest(const TPoint &position) override;

  void ApplyPaintTransform(std::shared_ptr<RenderObject> child, TM33 &transform) const override;

 protected:
  explicit FollowerObject(std::shared_ptr<LayerLink> link, bool show_when_unlinked = true,
                               const TPoint &offset = TPoint::Make(0, 0));

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

  bool AlwaysNeedsCompositing() override { return true; }

 private:
  std::shared_ptr<LayerLink> link_ = nullptr;
  TPoint follow_offset_ = TPoint::Make(0, 0);
  bool show_when_unlinked_ = true;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::FollowerObject, bases<tdfcore::RenderObject>)
  TDF_REFL_MEMBER(link_);
  TDF_REFL_MEMBER(follow_offset_);
  TDF_REFL_MEMBER(show_when_unlinked_);
TDF_REFL_END(tdfcore::FollowerObject)
