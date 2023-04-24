//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/data.h"
#include "core/common/size.h"

namespace tdfcore {

/**
 * @brief 图片编码格式
 */
enum class ImageCodecFormat : uint8_t {
  kUnknown,
  kPNG,
  kJPEG,
  kWEBP,
  kBMP,
  kGIF,
};

/**
 * @brief 检测图片编码格式
 */
ImageCodecFormat CheckImageCodecFormat(const std::shared_ptr<TData> &data);

/**
 * @brief 图片方向
 */
enum class ImageOrientation : int {
  // 正向
  kUp = 1,
  // 正向，水平翻转
  kUpMirrored = 2,
  // 180°旋转
  kDown = 3,
  // 180°旋转，再水平翻转
  kDownMirrored = 4,
  // 逆时针90°旋转，再垂直翻转
  kLeftMirrored = 5,
  // 顺时针90°旋转
  kRight = 6,
  // 顺时针90°旋转，再垂直翻转
  kRightMirrored = 7,
  // 逆时针90°旋转
  kLeft = 8,
};

/**
 * @brief 依据图片方向，给出修正后的最终图片尺寸
 * @param orientation 图片方向
 * @param codec_dimensions 图片编码尺寸
 */
ISize GetImageDisplayDimensions(ImageOrientation orientation, const ISize &codec_dimensions);

}  // namespace tdfcore
