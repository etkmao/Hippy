//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/image.h"
#include "tdfui/image/image_data.h"

namespace tdfcore {

/**
 * @brief 【基类】图像数据的缓存管理器
 * @details 对于相同的Image，可存入多个不同规格的瞬态数据；
 *          在获取该Image的瞬态数据时，可通过自定义的匹配算法，从多个不同规格的瞬态数据中挑选一个
 */
class ImageDataCache : public Object {
 public:
  ~ImageDataCache() override = default;

  /**
   * @brief 存入图像数据
   * @param image 关联的图像对象（用于建立索引）
   * @param image_info （待存入的图像数据）基础信息
   * @param frame （待存入的图像数据）单帧数据
   * @param raster_info （待存入的图像数据）光栅化操作参数信息
   * @return 经由image_info、frame、raster_info创建出来的ImageData实例，该实例受到ImageDataManager管控，能触发相应的缓存算法
   */
  virtual std::shared_ptr<ImageData> Store(const std::shared_ptr<Image> &image, const ImageData::ImageInfo &image_info,
                                           const ImageData::Frame &frame, const ImageData::RasterInfo &raster_info) = 0;

  /**
   * @brief ImageDataManager中存储的单个图像数据项的基本信息
   */
  struct Entry {
    // 图像实际尺寸
    ISize dimensions = ISize::MakeEmpty();

    // 光栅化操作详情
    ImageData::RasterInfo raster_info;

    // 【快捷方法】对比Entry中的processors与调用者指定的processors是否完全一致
    inline bool MatchesProcessors(const std::vector<std::shared_ptr<ImageProcessor>> &processors) const {
      if (processors.size() != raster_info.processors.size()) {
        return false;
      }
      for (size_t i = 0; i < processors.size(); ++i) {
        if (processors[i]->Descriptor() != raster_info.processors[i]->Descriptor()) {
          return false;
        }
      }
      return true;
    }
  };

  /**
   * @brief 匹配器
   * @param entries 备选匹配项列表
   * @return entries中被匹配中的项目的索引；当返回值超出entries的有效索引范围时（例如返回entries.size），则表示无匹配项
   */
  using Matcher = std::function<size_t(const std::vector<Entry> &entries)>;

  /**
   * @brief 查询匹配并获取ImageData
   * @param image 关联的图像对象
   * @param matcher 匹配器
   * @return 返回管理器中存在的符合匹配条件的ImageData，若无匹配数据，则返回nullptr
   */
  virtual std::shared_ptr<ImageData> Load(const std::shared_ptr<Image> &image, uint64_t frame_index,
                                          const Matcher &matcher) = 0;

  virtual void Reset() = 0;
};

}  // namespace tdfcore
