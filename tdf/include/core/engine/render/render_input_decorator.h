//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <utility>
#include "core/engine/render/render_object.h"

namespace tdfcore {

/**
 * @brief 文字输入框节点
 */
class RenderInputDecorator : public RenderObject {
  TDF_REFF_CLASS_META(RenderInputDecorator)

 public:
  /**
   * @brief 设置输入框外部的图标
   */
  void SetIcon(std::shared_ptr<RenderObject> icon);

  /**
   * @brief 设置输入内容
   * @param input
   */
  void SetInput(std::shared_ptr<RenderObject> input);

  /**
   * @brief 设置输入框的描述，获取焦点时悬浮到某位置
   */
  void SetLabel(std::shared_ptr<RenderObject> label);

  /**
   * @brief 设置提示文本
   */
  void SetHint(std::shared_ptr<RenderObject> hint);

  /**
   * @brief 预填充的内容
   */
  void SetPrefix(std::shared_ptr<RenderObject> prefix);

  /**
   * @brief 设置位于输入框内部起始位置的图标
   */
  void SetPrefixIcon(std::shared_ptr<RenderObject> prefix_icon);

  /**
   * @brief 设置尾部填充的内容
   */
  void SetSuffix(std::shared_ptr<RenderObject> suffix);

  /**
   * @brief 设置输入框后面的图标
   */
  void SetSuffixIcon(std::shared_ptr<RenderObject> suffix_icon);

  /**
   * @brief 设置错误提示信息
   */
  void SetHelperError(std::shared_ptr<RenderObject> helper_error);

  /**
   * @brief 设置位于输入框右下方的内容
   */
  void SetCounter(std::shared_ptr<RenderObject> counter);

  /**
   * @brief 设置边框的容器，用于边框的动画效果
   */
  void SetContainer(std::shared_ptr<RenderObject> container);

  /**
   * @brief 设置描述Label的偏移量
   */
  void SetLabelOffset(const TPoint &offset);

  /**
   * @brief 设置描述Label的变换参数
   */
  void SetLabelTransform(const TM33 &transform);

 protected:
  RenderInputDecorator();

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  std::shared_ptr<RenderObject> icon_ = nullptr;
  std::shared_ptr<RenderObject> input_ = nullptr;
  std::shared_ptr<RenderObject> label_ = nullptr;
  std::shared_ptr<RenderObject> hint_ = nullptr;
  std::shared_ptr<RenderObject> prefix_ = nullptr;
  std::shared_ptr<RenderObject> prefix_icon_ = nullptr;
  std::shared_ptr<RenderObject> suffix_ = nullptr;
  std::shared_ptr<RenderObject> suffix_icon_ = nullptr;
  std::shared_ptr<RenderObject> helper_error_ = nullptr;
  std::shared_ptr<RenderObject> counter_ = nullptr;
  std::shared_ptr<RenderObject> container_ = nullptr;
  TPoint label_offset_ = TPoint::Make(0, 0);
  TM33 label_transform_ = TM33();

  std::shared_ptr<RenderObject> UpdateDecoratorChild(const std::shared_ptr<RenderObject>& old_child,
                                                     std::shared_ptr<RenderObject> new_child);

  void DoPaint(const std::shared_ptr<RenderObject>& child, PaintingContext &context, TPoint offset);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::RenderInputDecorator, bases<tdfcore::RenderObject>)
  TDF_REFL_MEMBER(label_offset_);
  TDF_REFL_MEMBER(label_transform_);
TDF_REFL_END(tdfcore::RenderInputDecorator)