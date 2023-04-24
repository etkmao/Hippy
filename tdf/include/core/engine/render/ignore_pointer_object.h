//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include "core/engine/render/render_object.h"

namespace tdfcore {

/**
 * @brief 忽略事件绘制对象,
 * 当处于 ignore 时, 当前对象子节点正常显示，但其自身以及子节点对所有触碰事件不响应。
 */
class IgnorePointerObject : public RenderObject {
  TDF_REFF_CLASS_META(IgnorePointerObject)
 public:
  /**
   * @brief 设置是否忽略
   */
  void SetIgnorePointer(bool is_ignore);

 protected:
  explicit IgnorePointerObject(bool is_ignore);

  /**
   * @brief 碰撞测试，把当前position位置中下所有 HitTestTarget 添加到 HitTestResult 中去。
   *
   * @param position 碰撞的位置
   * @param transform 相对于根节点的转变
   * @return 碰撞的结果集合
   */
  std::shared_ptr<HitTestResult> HitTest(const TPoint &position) override;

 private:
  bool is_ignore_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::IgnorePointerObject, bases<tdfcore::RenderObject>)
TDF_REFL_MEMBER(is_ignore_);
TDF_REFL_END(tdfcore::IgnorePointerObject)
