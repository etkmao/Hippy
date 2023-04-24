//
// Copyright (c) Tencent Corporation. All rights reserved.
//
//
#pragma once

#include "core/engine/render/render_property.h"
#include "core/engine/render/render_object.h"
namespace tdfcore {

/**
 * @brief 提供Offstage能力的修饰容器，Offstage RenderObject虽然在挂在RenderObject Tree上，但不会产生任何效果
 */
class OffstageObject : public RenderObject {
  TDF_REFF_CLASS_META(OffstageObject)

 public:
  /**
   * @brief 设置是否离开渲染链不继续渲染
   * @param is_offstage
   */
  void SetOffstage(bool is_offstage);

  /**
   * @brief 返回是否离开渲染链不继续渲染
   */
  bool IsOffstage() const;

  std::shared_ptr<HitTestResult> HitTest(const TPoint &position) override;

  TRect GetChildVisibleRect(const RenderObject &child, const TRect &rect) override;

 protected:
  explicit OffstageObject(bool is_offstage = false);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  bool is_offstage_ = false;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::OffstageObject, bases<tdfcore::RenderObject>)
  TDF_REFL_MEMBER(is_offstage_);
TDF_REFL_END(tdfcore::OffstageObject)
