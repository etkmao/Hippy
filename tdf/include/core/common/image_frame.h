//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/bitmap.h"
#include "core/common/time.h"
#include "core/engine/graphics/texture.h"

namespace tdfcore {

/**
 * @brief 图片帧（此为Image解码出的单帧数据）
 * @details 请根据具体场景，通过子类化的方式，自定义图片帧内部的具体数据类型，及其数据类型转化能力
 */
class ImageFrame : public Object {
 public:
  ~ImageFrame() override = default;

  /**
   * @brief 获取帧时长
   */
  TDuration GetDuration() const { return duration_; }

  /**
   * @brief 获取图片帧的尺寸
   */
  virtual ISize Dimensions() const { return ISize::MakeEmpty(); }

  /**
   * @brief 获取Bitmap类型的数据；如果本ImageFrame不能提供该类型数据，则返回nullptr
   */
  virtual std::shared_ptr<Bitmap> GetBitmap() const { return nullptr; }

  /**
   * @brief 获取Texture类型的数据；如果本ImageFrame不能提供该类型数据，则返回nullptr
   */
  virtual std::shared_ptr<Texture> GetTexture(const std::shared_ptr<Device> &device) const { return nullptr; }

 protected:
  explicit ImageFrame(const TDuration &duration);

 private:
  TDuration duration_ = kMaximumDuration;
};

/**
 * @brief 基于Bitmap类型的图片帧
 */
class BitmapImageFrame : public ImageFrame {
 public:
  ~BitmapImageFrame() override = default;

  /**
   * @brief 构造函数
   */
  explicit BitmapImageFrame(const std::shared_ptr<Bitmap> &bitmap, const TDuration &duration = kMaximumDuration);

  /**
   * @brief 获取图片帧的尺寸
   */
  ISize Dimensions() const override { return ISize::Make(bitmap_->Width(), bitmap_->Height()); }

  /**
   * @brief 获取Bitmap类型的数据
   */
  std::shared_ptr<Bitmap> GetBitmap() const override { return bitmap_; }

  /**
   * @brief 获取Texture类型的数据（将根据内部持有的PixelBuffer数据转化得到Texture数据）
   */
  std::shared_ptr<Texture> GetTexture(const std::shared_ptr<Device> &device) const override;

 private:
  std::shared_ptr<Bitmap> bitmap_;
};

/**
 * @brief 基于Texture类型的图片帧
 */
class TextureImageFrame : public ImageFrame {
 public:
  ~TextureImageFrame() override = default;

  /**
   * @brief 构造函数
   */
  explicit TextureImageFrame(const std::shared_ptr<Texture> &texture,
                             const TDuration &duration = kMaximumDuration);

  /**
   * @brief 获取图片帧的尺寸
   */
  ISize Dimensions() const override { return texture_->Dimensions(); }

  /**
   * @brief 获取Bitmap类型的数据
   */
  std::shared_ptr<Bitmap> GetBitmap() const override;

  /**
   * @brief 获取Texture类型的数据
   */
  std::shared_ptr<Texture> GetTexture(const std::shared_ptr<Device> &device) const override { return texture_; }

 private:
  std::shared_ptr<Texture> texture_;
};

}  // namespace tdfcore
