//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "tdfcodec/image_decoder.h"
#include "core/common/macro.h"
#include "core/platform/common/platform.h"
#if TDF_PLATFORM_ANDROID
#include "core/platform/android/jni/jni_java_ref.h"
#endif

namespace tdfcore {

/**
 * @brief 图片解码器工厂
 * @details 工厂为每一张输入图片，创建出一个能够解码该图片的ImageDecoder实例；
 *          具体创建什么类型的ImageDecorder以及怎样创建，一切由Factory来决定
 */
class ImageDecoderFactory : public Object {
 public:
  ~ImageDecoderFactory() override = default;

  /**
   * @brief 创建一个解码器实例
   * @param image_raw_data 图片数据
   * @return 解码器实例
   */
  virtual std::shared_ptr<ImageDecoder> CreateImageDecoder(const std::shared_ptr<TData> &image_raw_data) = 0;
};

// NDK直接调用BitmapFactory解码png图片有时像素格式为Bitmap.Config.ARGB_F16且无法转换，导致图片渲染异常，Android需通过java层中转。
// 因此临时注入一个默认工厂，单独针对Android平台下的png解码进行特殊处理。
// FIXME(willisdai): 临时方案，后续使用libpng实现解码器。
class DefaultImageDecoderFactory : public ImageDecoderFactory {
 public:
  DefaultImageDecoderFactory() {}
  ~DefaultImageDecoderFactory() override = default;
  void Init() override;
  std::shared_ptr<ImageDecoder> CreateImageDecoder(const std::shared_ptr<TData> &image_raw_data) override;

 private:
  std::weak_ptr<Platform> platform_;
};

#if TDF_PLATFORM_ANDROID
class PngDecoderAndroid : public ImageDecoder {
 public:
  ~PngDecoderAndroid() override = default;
  uint64_t GetFrameCount() const override { return 1; }
  ISize GetImageDimensions() const override { return dimensions_; }
  void Decode(const Options &options, const FinishCallback &callback) override;

 protected:
  explicit PngDecoderAndroid(const std::shared_ptr<::tdfcore::TData> &data, std::shared_ptr<Platform> platform);

 private:
  std::shared_ptr<::tdfcore::JavaGlobalRef<jbyteArray>> data_;
  size_t data_size_;
  ISize dimensions_ = ISize::MakeEmpty();
  std::weak_ptr<Platform> platform_;

  FRIEND_OF_TDF_ALLOC
};
#endif

}  // namespace tdfcore
