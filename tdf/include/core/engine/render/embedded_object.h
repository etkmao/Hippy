//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <string_view>
#include "core/engine/render/render_object.h"

namespace tdfcore {
/**
 * @brief 可以嵌入到PlatformView的渲染对象
 */
class EmbeddedObject : public RenderObject {
  TDF_REFF_CLASS_META(EmbeddedObject)

 public:
  /**
   * EmbeddedObject固有缩放倍率，平台侧会根以EmbeddedInfo::rect * __intrinsic_scale为大小来初始化平台侧EmbeddedObject。
   * 例如Android平台的View
   * 见腾讯文档编辑中台embedded webview用法。
   */
  [[maybe_unused]] inline static std::string kPropertyIntrinsicScale = "__intrinsic_scale";

  /**
   * @brief 设置view属性
   * @param property 属性map容器
   */
  void SetProperty(const std::map<std::string, std::string> &property);

  /**
   * 设置是否使用自定义的HitTest区域，以及具体的区域信息
   */
  void SetHitTestRect(std::optional<TRect> rect_) { hit_test_rect_ = rect_; }

 protected:
  explicit EmbeddedObject(int64_t id, const std::string &type);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

  std::shared_ptr<HitTestResult> HitTest(const TPoint &position) override;

 protected:
  std::string type_;
  std::optional<TRect> hit_test_rect_;
  std::map<std::string, std::string> property_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::EmbeddedObject, bases<tdfcore::RenderObject>)
TDF_REFL_MEMBER(type_);
TDF_REFL_END(tdfcore::EmbeddedObject)
