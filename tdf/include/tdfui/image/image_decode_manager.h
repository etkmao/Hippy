//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <unordered_set>
#include "core/common/image.h"
#include "core/engine/schedule/scheduler.h"
#include "core/engine/schedule/task_driver.h"
#include "core/engine/schedule/task_runner.h"
#include "tdfcodec/image_decoder.h"
#include "tdfui/image/image_decoder_factory.h"
#include "tdfui/image/image_options.h"

namespace tdfcore {

class ImageDecodeManager : public Object {
 public:
  ~ImageDecodeManager() override;

  /**
   * @brief 构造函数
   * @param scheduler 通过指定的Scheduler去创建TaskRunner与TaskDriver
   * @param factory 设置自定义的ImageDecoder创建器；若为nullptr，则表示使用默认的创建器
   */
  explicit ImageDecodeManager(const std::shared_ptr<Scheduler> &scheduler,
                              const std::shared_ptr<ImageDecoderFactory> &factory = nullptr);

  /**
   * @brief 结束所有任务
   * @note 请在ImageDecodeManager析构之前，主动调用Terminate方法
   */
  void Terminate();

  /**
   * @brief 帧解码回调函数
   * @param image_frame 解码出的图像帧数据
   * @param decoder 解码时用到的解码器对象
   */
  using FinishCallback =
      std::function<void(const std::shared_ptr<ImageFrame> &image_frame, const std::shared_ptr<ImageDecoder> &decoder)>;

  /**
   * @brief 解码图片中的指定帧
   * @param image_raw_data 图片数据
   * @param frame_index 帧索引；对于普通单帧图片，只需填0
   * @param scale_options 缩放配置
   * @param callback 解码完成回调
   * @return Task实例
   */
  std::shared_ptr<Task> Decode(const std::shared_ptr<TData> &image_raw_data, uint64_t frame_index,
                               const ImageScaleOptions &scale_options, const FinishCallback &callback);

  /**
   * @brief 解码图片中的指定帧
   * @param decoder 使用指定的解码器
   * @param frame_index 帧索引；对于普通单帧图片，只需填0
   * @param scale_options 缩放配置
   * @param callback 解码完成回调
   * @return Task实例
   */
  std::shared_ptr<Task> Decode(const std::shared_ptr<ImageDecoder> &decoder, uint64_t frame_index,
                               const ImageScaleOptions &scale_options, const FinishCallback &callback);

  void SetFactory(const std::shared_ptr<ImageDecoderFactory> &factory) { decoder_factory_ = factory; }

 private:
  std::atomic_bool is_terminated_ = false;
  std::shared_ptr<TaskRunner> task_runner_;
  std::unordered_set<std::shared_ptr<TaskDriver>> task_drivers_;
  std::shared_ptr<ImageDecoderFactory> decoder_factory_;

  static ImageDecoder::Options MakeImageDecoderOptions(const std::shared_ptr<ImageDecoder> &decoder,
                                                       uint64_t frame_index, const ImageScaleOptions &options);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
