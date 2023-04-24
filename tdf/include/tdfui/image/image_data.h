//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/object.h"
#include "core/common/size.h"
#include "core/common/time.h"
#include "core/engine/render/image_object.h"
#include "tdfui/image/image_processor.h"

namespace tdfcore {

/**
 * @brief ImageData用于记录单个Image加载与解码后得到的瞬态数据：其中包含该Image的原始尺寸、总帧数，以及一张指定的帧数据
 */
class ImageData : public Object {
 public:
  ~ImageData() override = default;

  /**
   * @brief 图像基础信息
   */
  struct ImageInfo {
    // 原始尺寸
    ISize dimensions = ISize::MakeEmpty();

    // 总帧数
    uint64_t total_frame_count = 0;
  };

  /**
   * @brief 单帧数据
   */
  struct Frame {
    // 索引
    uint64_t index = 0;

    // 时长
    TDuration duration = kMaximumDuration;

    // 光栅化后的最终纹理：此为原图经过剪裁、应用滤镜等等操作之后，得到的最终图像纹理
    std::shared_ptr<Texture> texture = nullptr;
  };

  /**
   * @brief 光栅化操作参数信息
   */
  struct RasterInfo {
    // 在整个光栅化操作过程中，是否曾经改变过图像的原始尺寸
    bool has_resized = false;

    // 后置图像处理
    std::vector<std::shared_ptr<ImageProcessor>> processors;

    // 是否为原图（即未发生过剪裁、缩放、图像处理等）
    bool IsOriginal() const { return !has_resized && processors.empty(); }
  };

  /**
   * @brief 构造函数
   * @param image_info 图像基础信息
   * @param frame 单帧数据
   * @param raster_info 光栅化操作参数信息
   */
  ImageData(const ImageData::ImageInfo &image_info, const ImageData::Frame &frame,
            const ImageData::RasterInfo raster_info);

  /**
   * @brief 图像基础信息
   */
  const ImageData::ImageInfo &GetImageInfo() const { return image_info_; }

  /**
   * @brief 当前帧数据
   */
  const ImageData::Frame &GetFrame() const { return frame_; }

  /**
   * @brief 光栅化操作参数信息
   */
  const ImageData::RasterInfo &GetRasterInfo() const { return raster_info_; }

  /**
   * @brief 图片数据占用的内存字节数
   */
  uint64_t MemorySize() const;

 private:
  ImageData::ImageInfo image_info_;
  ImageData::Frame frame_;
  ImageData::RasterInfo raster_info_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
