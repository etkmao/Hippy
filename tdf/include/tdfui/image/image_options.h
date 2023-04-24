//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/data.h"
#include "tdfui/image/image_data_cache.h"
#include "tdfui/image/image_processor.h"

namespace tdfcore {

/**
 * @brief 图片缩放配置参数
 */
struct ImageScaleOptions {
  // 期望的图片尺寸；当该尺寸为空时，表示维持图片的实际尺寸，不做任何尺寸上的处理
  ISize preferred_dimensions = ISize::MakeEmpty();

  // 当图片的实际尺寸与期望尺寸（prefer_size）不一致时，采用以下方式进行缩放适配
  ScaleType type = ScaleType::kStretch;
};

/**
 * @brief 图片数据加载选项
 */
struct ImageOptions {
  // 帧索引
  uint64_t frame_index = 0;

  // 缩放配置
  ImageScaleOptions scale;

  // 使用指定的缓存数据匹配算法；若为nullptr，则表示采用默认的算法，即下方的MatchDefaultImageData()算法
  ImageDataCache::Matcher image_data_matcher;

  // 后置图像处理操作
  std::vector<std::shared_ptr<ImageProcessor>> processors;
};

/**
 * @brief 默认的图片数据缓存模糊匹配算法
 * @param entries （当前缓存中）备选项
 * @param options （待匹配的图片）数据加载选项
 * @return entries中被匹配中的项目的索引；当返回值超出entries的有效索引范围时（例如返回entries.size），则表示无匹配项
 */
size_t MatchDefaultImageData(const std::vector<ImageDataCache::Entry> &entries, const ImageOptions &options);

}  // namespace tdfcore
