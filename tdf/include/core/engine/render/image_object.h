//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/property.h"
#include "core/engine/graphics/texture.h"
#include "core/engine/render/render_object.h"
namespace tdfcore {
/**
 * @brief 缩放类型
 */
enum class ScaleType : uint8_t { kStretch, kAspectFit, kAspectFill };

/**
 * @brief 图片渲染对象
 */
class ImageObject : public RenderObject {
  TDF_REFF_CLASS_META(ImageObject)

 public:
  /**
   * @brief 设置缩放类型
   * @param type 缩放类型
   */
  void SetScaleType(ScaleType type);

  /**
   * @brief 设置图像数据
   * @param image 图像数据
   */
  void SetTexture(std::shared_ptr<Texture> texture);

  /**
   * @brief 获得缩放类型
   */
  ScaleType GetScaleType();

 protected:
  explicit ImageObject(const std::shared_ptr<Texture> &image = nullptr, ScaleType scale_type = ScaleType::kStretch);

  void OnPaint(PaintingContext &context, const TPoint &offset) override;

 private:
  std::shared_ptr<Texture> texture_ = nullptr;
  ScaleType scale_type_ = ScaleType::kStretch;
  TRect image_rect_ = TRect::MakeEmpty();
  TM33 matrix_ = TM33::I();
  TPaint paint_;

  void UpdateMatrix(TPoint offset);

  FRIEND_OF_TDF_ALLOC;
};

template <>
class TypeProperty<ScaleType> : public Property {
  TYPE_PROPERTY_INSTANTIATION(ScaleType)
  std::string ValueToString() const override {
    switch (value_) {
      case tdfcore::ScaleType::kStretch:
        return R"("stretch")";
      case tdfcore::ScaleType::kAspectFit:
        return R"("aspect_fit")";
      case tdfcore::ScaleType::kAspectFill:
        return R"("aspect_fill")";
    }
  }
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ImageObject, bases<tdfcore::RenderObject>)
TDF_REFL_MEMBER(scale_type_);
TDF_REFL_MEMBER(texture_);
TDF_REFL_MEMBER(image_rect_);
TDF_REFL_MEMBER(matrix_);
TDF_REFL_END(tdfcore::Image)
