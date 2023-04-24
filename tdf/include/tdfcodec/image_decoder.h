//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/data.h"
#include "core/common/image_frame.h"
#include "tdfcodec/image_codec.h"

namespace tdfcore {

/**
 * @brief 图片解码器
 * @details 一个ImageDecoder实例只负责一张图片的解码工作；
 *   快捷使用：
 *     直接使用ImageDecoder的各个static方法，可创建默认的解码器实例（Make方法）、或直接解码单帧图像数据（Decode方法）
 *   子类化：
 *     ImageDecoder应当保留该图片的解码上下文context，以便能尽快解码指定帧的图片数据，这对于拥有多帧的动图来说尤为重要；
 *     ImageDecoder的实现方式没有限制，它既可以是图片格式通吃的解码器，也可以是只针对单一图片格式的解码器，
 *     既可以是基于NativeAPI实现的解码器，也可以是跨平台的基于第三方库实现的解码器；
 */
class ImageDecoder : public Object {
 public:
  ~ImageDecoder() override = default;

  /**
   * @brief 获取帧的总数
   */
  virtual uint64_t GetFrameCount() const = 0;

  /**
   * @brief 获取原图尺寸
   * @note 在未进行过解码操作前（即未调用过Decode方法前），有些解码器并不能提前给出原图尺寸，此时会返回零尺寸
   */
  virtual ISize GetImageDimensions() const = 0;

  /**
   * @brief 控制图片解码的各配置参量
   */
  struct Options {
    /**
     * @brief 生成配置
     * @param frame_index 帧索引；对于普通单帧图片，只需填0
     */
    static Options MakeFrameIndex(uint64_t frame_index) { return {frame_index, 1}; }

    /**
     * @brief 帧索引；对于普通单帧图片，只需填0
     */
    uint64_t frame_index = 0;

    /**
     * @brief 向下采样率；该参数将提示解码器需要在解码图片的过程中，直接对图片做向下采样，以影响最终的图片输出尺寸
     *        通常传值2的幂次，表示期望解码出的图片长宽为原图的1/sample_factor；小于等于1时将视为不进行向下采样
     * @note 该参数是"提示性参数"，能否生效、以及实际效果如何，取决于图片格式（目前仅PNG与JPEG支持）、以及解码器的具体实现
     *       例如基于Android与iOS平台原生API所实现的解码器，只会向下取整到2的幂次；基于某些第三方库实现的解码器，不一定支持该参数
     */
    int sample_factor = 1;
  };

  /**
   * @brief 帧解码回调函数
   * @param image_frame 解码后的帧数据
   */
  using FinishCallback = std::function<void(const std::shared_ptr<ImageFrame> &image_frame)>;

  /**
   * @brief 解码指定帧
   * @param options 配置参量
   * @param callback 解码完成回调
   */
  virtual void Decode(const Options &options, const FinishCallback &callback) = 0;

  /**
   * @brief 【快捷方法】创建一个默认的图片解码器实例
   * @param data 图片文件数据
   * @return 图片解码器
   */
  static std::shared_ptr<ImageDecoder> Make(const std::shared_ptr<TData> &data);

  /**
   * @brief 【快捷方法】同步解码图片的指定一帧
   * @param data 图片文件数据
   * @param frame_index 帧索引；对于普通单帧图片，只需填0
   * @param sample_factor 向下采样率，请参阅Options.sample_factor
   * @return 解码后的帧数据
   */
  static std::shared_ptr<ImageFrame> Decode(const std::shared_ptr<TData> &data, uint64_t frame_index = 0,
                                            int sample_factor = 1);
};

}  // namespace tdfcore
