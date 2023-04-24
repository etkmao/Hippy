//
// Copyright © 2020 com.tencent. All rights reserved.

//

#pragma once

#include "core/common/m33.h"
#include "core/engine/render/render_object.h"

namespace tdfcore {
/**
 * @brief 三维矩阵变换修饰渲染对象
 * 节点可以实现3D旋转、缩放和平移操作
 */
class TransformObject : public RenderObject {
TDF_REFF_CLASS_META(TransformObject)

 public:
  /**
   * @brief 设置变换矩阵
   * @param matrix
   */
  void SetTransform(const TM33 &matrix);

  /**
  * @brief 返回变换矩阵
  */
  const TM33 &GetTransform() const;

  /**
   * @brief 变换矩阵设置为单位矩阵
   */
  void SetIdentity();

  /**
   * @brief 围绕x轴旋转
   * @param degree 角度
   */
  void RotateX(TScalar degree);

  /**
   * @brief 围绕y轴旋转
   * @param degree 角度
   */
  void RotateY(TScalar degree);

  /**
   * @brief 围绕z轴旋转
   * @param degree 角度
   */
  void RotateZ(TScalar degree);

  /**
   * @brief 围绕axis轴旋转
   * @param axis 旋转轴
   * @param degree 角度
   */
  void RotateAxis(const TV3 &axis, TScalar degree);

  /**
   * @brief 平移
   * @param x 平移x分量
   * @param y 平移y分量
   * @param z 平移z分量
   */
  void Translate(TScalar x, TScalar y, TScalar z = 0.0);

  /**
   * @brief 缩放
   * @param x 缩放x分量
   * @param y 缩放y分量
   * @param z 缩放z分量
   */
  void Scale(TScalar x, TScalar y, TScalar z = 1.0);

  void ApplyPaintTransform(std::shared_ptr<RenderObject> child, TM33 &transform) const override;

  /**
   * @brief 碰撞测试，把当前position位置中下所有 HitTestTarget 添加到 HitTestResult 中去。
   *
   * @param position 碰撞的位置
   * @param transform 相对于根节点的转变
   * @return 碰撞结果集合
   */
  std::shared_ptr<HitTestResult> HitTest(const TPoint &position) override;

  TRect GetChildVisibleRect(const RenderObject &child, const TRect &rect) override;

 protected:
  explicit TransformObject(const TM33 &transform = TM33(), const TV3 &axis = {0.5, 0.5, 0.5});

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  TM33 GetEffectiveTransform() const;

  TM33 transform_;
  TV3 axis_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::TransformObject, bases < tdfcore::RenderObject>)
  TDF_REFL_MEMBER(transform_);
TDF_REFL_END(tdfcore::TransformObject)
