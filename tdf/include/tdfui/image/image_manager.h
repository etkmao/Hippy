//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <unordered_map>
#include "core/engine/schedule/scheduler.h"
#include "tdfui/image/image_data_cache.h"
#include "tdfui/image/image_decode_manager.h"
#include "tdfui/image/image_load_manager.h"
#include "tdfui/image/image_raster_manager.h"
#include "tdfui/image/image_task.h"

namespace tdfcore {

/**
 * @brief 图片数据加载管理器
 */
class ImageManager : public Object, public std::enable_shared_from_this<ImageManager> {
 public:
  ~ImageManager() override;

  /**
   * @brief ImageManager的配置选项
   */
  struct Configuration {
    /**
     * @brief 图像数据缓存器；
     *        若为nullptr，则会默认创建一个DefaultImageDataCache实例
     */
    std::shared_ptr<ImageDataCache> data_cache;

    // @brief 解码器工厂
    std::shared_ptr<ImageDecoderFactory> decoder_factory;

    /**
     * @brief 创建一个默认配置
     */
    static Configuration MakeDefault() { return Configuration(); }
  };

  /**
   * @brief 构造函数
   * @param shell Shell实例
   * @param configuration 配置选项
   */
  explicit ImageManager(const std::shared_ptr<Shell> &shell, const Configuration &configuration = Configuration());

  /**
   * @brief 结束所有任务，并进入不可用状态
   * @note 请在ImageManager析构之前，主动调用Terminate方法
   */
  void Terminate();

  /**
   * @brief 是否为不可用状态
   */
  bool IsTerminated() const { return is_terminated_; }

  /**
   * @brief 查询匹配缓存内的图片数据
   * @param image 待查询匹配的Image
   * @param options 图片数据加载选项
   * @return 若缓存中存在匹配的图片数据，则返回该数据，否则返回nullptr
   */
  std::shared_ptr<ImageData> Query(const std::shared_ptr<Image> &image, const ImageOptions &options);

  /**
   * @brief 异步加载图片数据（单帧）
   * @details 本方法返回的ImageLoadTask实例中保存有解码上下文环境，若Image为多帧图，且调用者希望继续加载其它的后续帧，
   *          则高效的做法是，一直持有ImageLoadTask实例，继续再次调用其Load方法，此做法将受到解码上下文环境的加持
   * @param image 待加载数据的Image
   * @param options 图片数据加载选项
   * @param finish_callback 加载完成回调函数
   * @return 任务（可通过它的Cancel方法取消）
   */
  std::shared_ptr<ImageLoadTask> Load(const std::shared_ptr<Image> &image, const ImageOptions &options,
                                      const ImageLoadTask::FinishCallback &finish_callback);

  /**
   * @brief 同步加载图片数据（单帧）
   * @param image 待加载数据的Image
   * @param options 图片数据加载选项
   * @return 图片数据
   */
  inline std::shared_ptr<ImageData> Load(const std::shared_ptr<Image> &image, const ImageOptions &options) {
    std::promise<std::shared_ptr<ImageData>> promise;
    Load(image, options, [&promise](const std::shared_ptr<ImageData> &data, ImageError error) {
      promise.set_value((error == ImageError::kNone) ? data : nullptr);
    });
    return promise.get_future().get();
  }

  /**
   * @brief 异步解码图片（单帧）
   * @details 本方法返回的ImageDecodeTask实例中保存有解码上下文环境，若Image为多帧图，且调用者希望继续解码其它的后续帧，
   *          则高效的做法是，一直持有ImageDecodeTask实例，继续再次调用其Decode方法，此做法将受到解码上下文环境的加持
   * @param image 待解码的Image
   * @param frame_index 帧索引
   * @param finish_callback 图片解码回调函数
   * @return 任务（可通过它的Cancel方法取消，也可通过它的Decode方法继续解码其它帧）
   */
  std::shared_ptr<ImageDecodeTask> Decode(const std::shared_ptr<Image> &image, uint64_t frame_index,
                                          const ImageDecodeManager::FinishCallback &finish_callback,
                                          const ImageScaleOptions &scale_opt = ImageScaleOptions());

  /**
   * @brief 异步解码图片（全部帧）
   * @param image 待解码的Image
   * @param finish_callback 图片解码回调函数
   * @return 任务（可通过它的Cancel方法取消）
   */
  std::shared_ptr<ImageDecodeAllFramesTask> Decode(const std::shared_ptr<Image> &image,
                                                   const ImageDecodeAllFramesTask::FinishCallback &cb,
                                                   const ImageScaleOptions &scale_opt = ImageScaleOptions());

  /**
   * @brief 获取持有的Shell对象
   */
  std::shared_ptr<Shell> GetShell() const { return shell_; }

  /**
   * @brief 获取持有的ImageLoadManager对象
   */
  std::shared_ptr<ImageLoadManager> GetImageLoadManager() const { return load_manager_; }

  /**
   * @brief 获取持有的ImageDecodeManager对象
   */
  std::shared_ptr<ImageDecodeManager> GetImageDecodeManager() const { return decode_manager_; }

  /**
   * @brief 获取持有的ImageRasterManager对象
   */
  std::shared_ptr<ImageRasterManager> GetImageRasterManager() const { return raster_manager_; }

  /**
   * @brief 获取持有的ImageDataCache对象
   */
  std::shared_ptr<ImageDataCache> GetImageDataCache() const { return data_cache_; }

 private:
  std::atomic_bool is_terminated_ = false;
  std::shared_ptr<Shell> shell_;
  std::shared_ptr<ImageLoadManager> load_manager_;
  std::shared_ptr<ImageDecodeManager> decode_manager_;
  std::shared_ptr<ImageRasterManager> raster_manager_;
  std::shared_ptr<ImageDataCache> data_cache_;
  std::unordered_map<int64_t, std::shared_ptr<ImageTask>> tasks_;
  uint64_t task_counter_ = 0;
  std::mutex task_mutex_;

  uint64_t AddTask(const std::shared_ptr<ImageTask> &task);
  std::shared_ptr<ImageTask> RemoveTask(uint64_t task_id);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
