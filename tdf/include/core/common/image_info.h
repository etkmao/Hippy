//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <cinttypes>
#include <cstddef>

namespace tdfcore {

enum class AlphaType {
  // 未初始化
  kUnknown,

  // 不透明；表示Alpha通道无效
  kOpaque,

  // 预乘；表示其它色彩通道的色值已预先乘过Alpha值
  kPremultiplied,

  // 非预乘；表示其它色彩通道的色值与Alpha值完全独立不相干
  kUnpremultiplied,
};

enum class ColorType {
  kUnknown,
  kALPHA_8,
  kRGBA_8888,
  kBGRA_8888,
};

class ImageInfo {
 public:
  ImageInfo() = default;

  /**
   * @brief 创建ImageInfo
   * @param width 宽度
   * @param height 高度
   * @param color_type 色彩模式
   * @param alpha_type 透明模式
   * @param row_bytes 图像单行所占用的内存字节数；此值不可小于MinRowBytes的计算值（即图像数据在存储时可以存在padding）；
   *                  当本参数填0时，表示采用MinRowBytes的值
   */
  static ImageInfo Make(uint32_t width, uint32_t height, ColorType color_type,
                        AlphaType alpha_type = AlphaType::kPremultiplied, size_t row_bytes = 0);

  /**
   * @brief 获取指定色彩模式下的单个像素所占用的内存字节数
   */
  static size_t BytesPerPixel(ColorType color_type);

  /**
   * @brief 获取宽度
   */
  constexpr uint32_t Width() const { return width_; }

  /**
   * @brief 获取高度
   */
  constexpr uint32_t Height() const { return height_; }

  /**
   * @brief 获取色彩模式
   */
  constexpr ColorType GetColorType() const { return color_type_; }

  /**
   * @brief 获取透明模式
   */
  constexpr AlphaType GetAlphaType() const { return alpha_type_; }

  /**
   * @brief 获取图像单行所占用的内存字节数
   */
  constexpr size_t RowBytes() const { return row_bytes_; }

  /**
   * @brief 获取图像单行所占用的（理论上的）最小内存字节数
   */
  size_t MinRowBytes() const { return width_ * BytesPerPixel(); }

  /**
   * @brief 获取整个图像所占用的内存字节数
   */
  size_t ByteSize() const { return row_bytes_ * height_; }

  /**
   * @brief 获取单个像素所占用的内存字节数
   */
  size_t BytesPerPixel() const { return BytesPerPixel(color_type_); }

 private:
  ImageInfo(uint32_t width, uint32_t height, ColorType color_type, AlphaType alpha_type, size_t row_bytes)
      : width_(width), height_(height), color_type_(color_type), alpha_type_(alpha_type), row_bytes_(row_bytes) {}

  uint32_t width_ = 0;
  uint32_t height_ = 0;
  ColorType color_type_ = ColorType::kUnknown;
  AlphaType alpha_type_ = AlphaType::kUnknown;
  size_t row_bytes_ = 0;
};

}  // namespace tdfcore