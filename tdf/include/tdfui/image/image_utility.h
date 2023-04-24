//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/data.h"
#include "core/common/object.h"
#include "core/common/size.h"
#include "core/common/time.h"
#include "core/engine/render/image_object.h"
#include "tdfcodec/image_decoder.h"

namespace tdfcore {

/**
 * @brief 图片通用工具类
 */
class ImageUtility {
 public:
  /**
   * @brief 分解图片URL
   * @param url
   * @return scheme & source
   */
  static std::pair<std::string, std::string> ParseUrl(const std::string &url);

  /**
   * @brief ScaleType转为String
   * @param scale_type
   * @return string
   */
  static std::string ScaleTypeToString(ScaleType scale_type);

  /**
   * @brief 将一个图像按照指定的缩放模式展示于一个视图中时，需要对该图像进行缩放的倍率
   *        倍率小于1，即图像需要缩小；倍率大于1，即图像需要放大
   * @param dimensions 图像尺寸
   * @param view_size 目标视图的尺寸
   * @param scale_type 缩放模式
   * @return 缩放比例
   */
  static TSize ComputeScales(const ISize &dimensions, const ISize &view_size, ScaleType scale_type);
};

}  // namespace tdfcore
