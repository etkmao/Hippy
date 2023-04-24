//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <mutex>
#include <unordered_map>
#include "core/common/lru_cache.h"
#include "tdfui/image/image_data_cache.h"

namespace tdfcore {

/**
 * @brief TDF框架默认提供的一套图片数据缓存管理器
 * @details
 *   缓存结构：
 *     一级缓存（trace_cache_）：此为一个弱引用map，记录了当前App内正在被使用的所有ImageData
 *     二级缓存（lru_cache_）：此为一个标准的LRU缓存
 *   缓存策略：
 *     （1）App内那些正在被使用的ImageData实例，它们绝不应当被算在缓存淘汰的行列里，且它们本身也正在被其它模块持有着，
 *         因此本Manager只需留一个弱引用即可，用一级缓存做标记即可
 *     （2）当App内其它模块均不再使用某个ImageData实例时，此时，本Manager将会回收此ImageData，将其从一级缓存中降级至二级缓存中，
 *         即二级缓存中记录的都是App内当前不再使用的ImageData，随着LRU规则，这些ImageData会逐渐被淘汰丢弃
 *     （3）当二级缓存内的某个ImageData实例被命中缓存，再次拿出来使用时，此时它将从二级缓存中晋升至一级缓存中
 *   分组查询：
 *     （1）考虑到有多帧图的情况，故将"Image中的每一独立帧"作为划分组的依据，组id也是通过Image联合帧索引一起生成出来的，
 *         见GenerateKeyGroup方法
 *     （2）每一组中的所有ImageData，都是某Image的某一固定帧的不同规格的图像数据
 *     （3）所谓不同规格，指的是尺寸不同、叠加的滤镜效果不同，等等
 */
class DefaultImageDataCache : public ImageDataCache, public std::enable_shared_from_this<DefaultImageDataCache> {
 public:
  ~DefaultImageDataCache() override = default;

  /**
   * @brief 构造函数
   * @param work_task_runner 指定任务执行专用的TaskRunner；
   *                         若指定了TaskRunner，则图片解码任务将全部投递至该TaskRunner，外界亦可自由掌控线程模型
   *                         若为nullptr，则ImageDecodeManager会自动创建一个内部专用TaskRunner，并采用自己的线程模型
   */
  explicit DefaultImageDataCache(uint64_t capacity);

  /**
   * @brief 存入图像数据
   * @param image 关联的图像对象（用于建立索引）
   * @param image_info （待存入的图像数据）基础信息
   * @param frame （待存入的图像数据）单帧数据
   * @param raster_info （待存入的图像数据）光栅化操作参数信息
   * @return 经由image_info、frame、raster_info创建出来的ImageData实例，该实例受到ImageDataCache管控，能触发相应的缓存算法
   */
  std::shared_ptr<ImageData> Store(const std::shared_ptr<Image> &image, const ImageData::ImageInfo &image_info,
                                   const ImageData::Frame &frame, const ImageData::RasterInfo &raster_info) override;

  /**
   * @brief 查询匹配并获取ImageData
   * @param image 关联的图像对象
   * @param matcher 匹配器
   * @return 返回管理器中存在的符合匹配条件的ImageData，若无匹配数据，则返回nullptr
   */
  virtual std::shared_ptr<ImageData> Load(const std::shared_ptr<Image> &image, uint64_t frame_index,
                                          const Matcher &matcher) override;

  virtual void Reset() override;

 private:
  struct Key {
    std::string group;
    std::string entry;
    std::string ToString() const { return group + "_" + entry; }
  };

  struct DestoryNotifier {
    Key key;
    std::function<void(const Key &key, const ImageData *)> callback;
  };

  std::recursive_mutex mutex_;
  std::unordered_map<std::string, std::weak_ptr<ImageData>> trace_cache_;
  LRUCache<std::string, std::shared_ptr<ImageData>> lru_cache_;
  using Entries = std::vector<std::pair<std::string, Entry>>;
  std::unordered_map<std::string, Entries> group_map_;
  std::unordered_map<const ImageData *, std::shared_ptr<DestoryNotifier>> notifier_map_;

  std::string GenerateKeyGroup(const std::shared_ptr<Image> &image, uint64_t frame_index);
  std::string GenerateKeyEntry(const Entry &entry);
  std::shared_ptr<ImageData> MakeImageData(const Key &key, const ImageData::ImageInfo &image_info,
                                           const ImageData::Frame &frame, const ImageData::RasterInfo &raster_info);
  std::shared_ptr<ImageData> Load(const Key &key);
  void Erase(const Key &key);
  void StoreToTraceCache(const Key &key, const std::shared_ptr<ImageData> &data);
  void StoreToLRUCache(const Key &key, const std::shared_ptr<ImageData> &data);
  void AddToGroup(const Key &key, const Entry &frame);
  void RemoveFromGroup(const Key &key);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore