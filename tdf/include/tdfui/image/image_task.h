//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/image.h"
#include "tdfui/image/image_data_cache.h"
#include "tdfui/image/image_decode_manager.h"
#include "tdfui/image/image_load_manager.h"
#include "tdfui/image/image_options.h"

namespace tdfcore {

class ImageManager;

/**
 * @brief 图片数据加载错误码
 */
enum class ImageError {
  // 没有错误，即成功
  kNone,

  // 取消造成的中断
  kCancelled,

  // ImageManager不可用
  kInvalidImageManager,

  // 加载图片的原始数据时发生错误
  kLoadRawDataFailed,

  // 解码图片时发生错误
  kDecodeFrameFailed,

  // 光栅化处理时发生错误
  kRasterFailed,
};

/**
 * @brief 【基类】图片处理任务；请使用各个子类
 */
class ImageTask : public Object, public std::enable_shared_from_this<ImageTask> {
 public:
  ~ImageTask() override = default;

  /**
   * @brief 是否正在执行
   */
  bool IsRunning() const { return is_running_; }

  /**
   * @brief 取消执行
   */
  void Cancel();

  /**
   * @brief 设置图片文件加载进度回调函数
   * @note 仅当发生了图片文件加载操作时，本Task才会发出此回调
   * @param callback 进度回调函数
   */
  void SetImageLoadProgressCallback(const ImageLoader::ProgressCallback &callback);

 protected:
  ImageTask(const std::shared_ptr<Image> &image, const std::shared_ptr<ImageManager> &manager);
  std::shared_ptr<ImageManager> GetImageManager() const;

  // 图片文件数据加载操作
  uint64_t LoadRawData(const std::string &url);

  // 解码操作
  std::shared_ptr<Task> DecodeFrame(const std::shared_ptr<TData> &data, const std::shared_ptr<ImageDecoder> &decoder,
                                    uint64_t frame_index, const ImageScaleOptions &scale_options);

  // 光栅化操作（包括上传纹理、剪裁、效果处理等）
  std::shared_ptr<Task> Raster(const std::shared_ptr<ImageFrame> &frame, const ImageOptions &options);

  // 尝试缓存入ImageDataCache中
  std::shared_ptr<ImageData> StoreImageData(const std::shared_ptr<Image> &image, const ImageOptions &options,
                                            const ImageData::ImageInfo &image_info, const ImageData::Frame &frame,
                                            const ImageData::RasterInfo &raster_info);

  // LoadRawData()成功时将触发此回调，子类重写，以决定下一步操作
  virtual void OnLoadRawDataFinished(const std::shared_ptr<TData> &data) {}

  // DecodeFrame()成功时将触发此回调，子类重写，以决定下一步操作
  virtual void OnDecodeFrameFinished(const std::shared_ptr<ImageFrame> &frame,
                                     const std::shared_ptr<ImageDecoder> &decoder,
                                     const ImageData::ImageInfo &image_info) {}

  // Raster()成功时将触发此回调，子类重写，以决定下一步操作
  virtual void OnRasterFinished(const std::shared_ptr<Texture> &texture, const ImageData::RasterInfo &raster_info) {}

  // 任意操作发生错误时将触发此回调，子类重写，以处理错误
  virtual void OnError(ImageError error) = 0;

  // 任务取消的具体操作
  virtual void OnCancel() = 0;

 protected:
  std::atomic_bool is_running_ = false;
  std::shared_ptr<Image> image_;
  std::weak_ptr<ImageManager> manager_;
  ImageLoader::ProgressCallback progress_callback_;
};

/**
 * @brief 全流程加载图片数据，给到最终的用于渲染的纹理
 */
class ImageLoadTask : public ImageTask {
 public:
  ~ImageLoadTask() override = default;

  /**
   * @brief 构造函数
   * @param image 待加载数据的图片
   * @param manager ImageManager实例
   */
  ImageLoadTask(const std::shared_ptr<Image> &image, const std::shared_ptr<ImageManager> &manager);

  /**
   * @brief 图片数据加载完成回调函数
   * @param data 图片数据
   * @param error 错误码
   */
  using FinishCallback = std::function<void(const std::shared_ptr<ImageData> &data, ImageError error)>;

  /**
   * @brief 加载
   * @param options 图片数据加载选项
   * @param finish_callback 加载完成回调函数
   */
  void Load(const ImageOptions &options, const FinishCallback &finish_callback);

 protected:
  void OnLoadRawDataFinished(const std::shared_ptr<TData> &data) override;
  void OnDecodeFrameFinished(const std::shared_ptr<ImageFrame> &frame, const std::shared_ptr<ImageDecoder> &decoder,
                             const ImageData::ImageInfo &image_info) override;
  void OnRasterFinished(const std::shared_ptr<Texture> &texture, const ImageData::RasterInfo &raster_info) override;
  void OnError(ImageError error) override;
  void OnCancel() override;
  void Finish(const std::shared_ptr<ImageData> &data, ImageError error);

 private:
  ImageOptions options_;
  // 与LoadRawData相关的状态量
  uint64_t load_callback_id_ = 0;
  // 与DecodeFrame相关的状态量
  std::shared_ptr<ImageDecoder> decoder_;
  std::shared_ptr<Task> decode_task_;
  std::shared_ptr<ImageFrame> frame_;
  // 与Raster相关的状态量
  std::shared_ptr<Task> raster_task_;
  // 与最终结果相关的状态量
  ImageData::ImageInfo image_info_;
  FinishCallback finish_callback_;
};

/**
 * @brief 图片解码单帧
 */
class ImageDecodeTask : public ImageTask {
 public:
  ~ImageDecodeTask() override = default;

  /**
   * @brief 构造函数
   * @param image 待解码的图片
   * @param manager ImageManager实例
   */
  ImageDecodeTask(const std::shared_ptr<Image> &image, const std::shared_ptr<ImageManager> &manager);

  /**
   * @brief 解码
   * @param frame_index 帧索引
   * @param decode_callback 图片解码回调函数
   */
  void Decode(uint64_t frame_index, const ImageDecodeManager::FinishCallback &cb,
              const ImageScaleOptions &scale_opt = ImageScaleOptions());

 protected:
  void OnLoadRawDataFinished(const std::shared_ptr<TData> &data) override;
  void OnDecodeFrameFinished(const std::shared_ptr<ImageFrame> &frame, const std::shared_ptr<ImageDecoder> &decoder,
                             const ImageData::ImageInfo &image_info) override;
  void OnError(ImageError error) override;
  void OnCancel() override;
  void Finish(const std::shared_ptr<ImageFrame> &frame, const std::shared_ptr<ImageDecoder> &decoder);

 private:
  // 与LoadRawData相关的状态量
  uint64_t load_callback_id_ = 0;
  // 与DecodeFrame相关的状态量
  uint64_t frame_index_;
  ImageScaleOptions scale_opt_;
  std::shared_ptr<ImageDecoder> decoder_;
  std::shared_ptr<Task> decode_task_;
  ImageDecodeManager::FinishCallback finish_callback_;
};

/**
 * @brief 图片解码全部帧
 */
class ImageDecodeAllFramesTask : public ImageTask {
 public:
  ~ImageDecodeAllFramesTask() override = default;

  /**
   * @brief 构造函数
   * @param image 待解码的图片
   * @param manager ImageManager实例
   */
  ImageDecodeAllFramesTask(const std::shared_ptr<Image> &image, const std::shared_ptr<ImageManager> &manager);

  /**
   * @brief 解码回调函数
   * @param frames 解码出的全部帧（按帧的原始顺序存储）
   * @param error 错误码
   */
  using FinishCallback = std::function<void(const std::vector<std::shared_ptr<ImageFrame>> &frames, ImageError error)>;

  /**
   * @brief 解码
   * @param finish_callback 图片解码回调函数
   */
  void Decode(const FinishCallback &cb, const ImageScaleOptions &scale_opt = ImageScaleOptions());

 protected:
  void OnLoadRawDataFinished(const std::shared_ptr<TData> &data) override;
  void OnDecodeFrameFinished(const std::shared_ptr<ImageFrame> &frame, const std::shared_ptr<ImageDecoder> &decoder,
                             const ImageData::ImageInfo &image_info) override;
  void OnError(ImageError error) override;
  void OnCancel() override;
  void Finish(const std::vector<std::shared_ptr<ImageFrame>> &frames, ImageError error);

 private:
  // 与LoadRawData相关的状态量
  uint64_t load_callback_id_ = 0;
  ImageScaleOptions scale_opt_;
  // 与DecodeFrame相关的状态量
  std::shared_ptr<Task> decode_task_;
  std::shared_ptr<ImageDecoder> decoder_;
  // 与最终结果相关的状态量
  std::vector<std::shared_ptr<ImageFrame>> frames_;
  FinishCallback finish_callback_;
};

}  // namespace tdfcore
