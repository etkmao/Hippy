//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/reflection.h"
#include "core/engine/graphics/texture.h"
#include "core/engine/render/image_object.h"

namespace tdfcore {

/**
 * @brief 图像处理器
 */
class ImageProcessor : public Object, public Reflectable {
  TDF_REFF_CLASS_META(ImageProcessor)

 public:
  ~ImageProcessor() override = default;

  /**
   * @brief 图像处理
   * @param input 原始图像纹理
   * @param context GPU上下文环境
   * @return 图像处理后的纹理
   */
  virtual std::shared_ptr<Texture> Process(const std::shared_ptr<Texture> &input,
                                           const std::shared_ptr<Device> &device) = 0;

  /**
   * @brief 关于当前实例的类型以及参数值的描述性字符串，可用于区别不同的实例
   */
  virtual std::string Descriptor() const = 0;
};

/**
 * @brief 图像缩放
 * @details 根据目标视图的尺寸以及指定的缩放模式，对图像进行整体缩小
 *          输出图像的尺寸不会大于原图像尺寸，且维持输出图像与原图像的宽高比一致
 */
class ScaleImageProcessor : public ImageProcessor {
  TDF_REFF_CLASS_META(ScaleImageProcessor)

 public:
  ~ScaleImageProcessor() override = default;

  /**
   * @brief 构造函数
   * @param view_size 目标视图的尺寸
   * @param scale_type 缩放模式
   */
  explicit ScaleImageProcessor(const ISize &view_size, ScaleType scale_type);

  /**
   * @brief 目标视图的尺寸
   */
  constexpr const ISize &ViewSize() const { return view_size_; }

  /**
   * @brief 缩放模式
   */
  ScaleType GetScaleType() const { return scale_type_; }

  /**
   * @brief 图像处理
   * @param input 原始图像纹理
   * @param context GPU上下文环境
   * @return 图像处理后的纹理
   */
  std::shared_ptr<Texture> Process(const std::shared_ptr<Texture> &input,
                                   const std::shared_ptr<Device> &device) override;

  /**
   * @brief 关于当前实例的类型以及参数值的描述性字符串，可用于区别不同的实例
   */
  std::string Descriptor() const override;

 private:
  ISize view_size_;
  ScaleType scale_type_;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 图像灰化
 */
class GrayscaleImageProcessor : public ImageProcessor {
  TDF_REFF_CLASS_META(GrayscaleImageProcessor)

 public:
  ~GrayscaleImageProcessor() override = default;

  /**
   * @brief 构造函数
   */
  GrayscaleImageProcessor();

  /**
   * @brief 图像处理
   * @param input 原始图像纹理
   * @param context GPU上下文环境
   * @return 图像处理后的纹理
   */
  std::shared_ptr<Texture> Process(const std::shared_ptr<Texture> &input,
                                   const std::shared_ptr<Device> &device) override;

  /**
   * @brief 关于当前实例的类型以及参数值的描述性字符串，可用于区别不同的实例
   */
  std::string Descriptor() const override;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 高斯模糊
 */
class GaussianBlurImageProcessor : public ImageProcessor {
  TDF_REFF_CLASS_META(GaussianBlurImageProcessor)

 public:
  ~GaussianBlurImageProcessor() override = default;

  /**
   * @brief 构造函数
   * @param sigma 高斯分布的标准差
   */
  explicit GaussianBlurImageProcessor(float sigma);

  /**
   * @brief 高斯分布的标准差
   */
  float Sigma() const { return sigma_; }

  /**
   * @brief 图像处理
   * @param input 原始图像纹理
   * @param context GPU上下文环境
   * @return 图像处理后的纹理
   */
  std::shared_ptr<Texture> Process(const std::shared_ptr<Texture> &input,
                                   const std::shared_ptr<Device> &device) override;

  /**
   * @brief 关于当前实例的类型以及参数值的描述性字符串，可用于区别不同的实例
   */
  std::string Descriptor() const override;

 private:
  float sigma_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ImageProcessor, bases<>)
TDF_REFL_END(tdfcore::ImageProcessor)
TDF_REFL_DEFINE(tdfcore::ScaleImageProcessor, bases<tdfcore::ImageProcessor>)
TDF_REFL_END(tdfcore::ScaleImageProcessor)
TDF_REFL_DEFINE(tdfcore::GrayscaleImageProcessor, bases<tdfcore::ImageProcessor>)
TDF_REFL_END(tdfcore::GrayscaleImageProcessor)
TDF_REFL_DEFINE(tdfcore::GaussianBlurImageProcessor, bases<tdfcore::ImageProcessor>)
TDF_REFL_END(tdfcore::GaussianBlurImageProcessor)
