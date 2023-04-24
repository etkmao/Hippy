//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <unordered_map>
#include "core/common/data.h"
#include "core/common/image_frame.h"
#include "tdfcodec/image_codec.h"

namespace tdfcore {

/**
 * @brief 图片编码器
 * @details
 *   一个ImageEncoder实例只负责一张图片的编码工作
 *   快捷使用：
 *     直接使用ImageEncoder的各个static方法，可创建默认的编码器实例（Make方法）、或直接编码单帧图片（Encode方法）
 *   子类化：
 *     ImageEncoder子类的实现方式没有限制，它既可以是图片格式通吃的编码器，也可以是只针对单一图片格式的编码器，
 *     既可以是基于NativeAPI实现的编码器，也可以是跨平台的基于第三方库实现的编码器；
 *   编码流程：
 *    （1）当ImageEncoder实例化时，需建立起图片编码上下文context，使得编码流程正式启动
 *    （2）每调用一次AddFrame()方法，就会编入一帧图像
 *    （3）调用FinishEncoding()方法，结束整个编码流程，并返回最终编码生成的图片文件数据
 */
class ImageEncoder : public Object {
 public:
  ~ImageEncoder() override = default;

  /**
   * @brief 控制图片编码器算法的各配置参量
   */
  struct Options {
    /**
     * @brief 默认配置
     */
    static Options MakeDefault() { return Options(); }

    /**
     * @brief 图像编码压缩质量，可用于JPEG格式；有效区间[0.0 ~ 1.0]
     */
    float compression_quality = 1.0;
  };

  /**
   * @brief 构造函数
   * @param format 图片格式
   * @param options 控制图片编码算法的各配置参量
   */
  ImageEncoder(ImageCodecFormat format, const Options &options);

  /**
   * @brief 编入一帧图像
   * @note 调用过FinishEncoding()之后，不可再调用本方法
   * @param image_frame 图片帧
   * @return 是否成功
   */
  virtual bool AddFrame(const std::shared_ptr<ImageFrame> &image_frame) = 0;

  /**
   * @brief 结束解码流程
   * @note 不允许重复调用本方法
   * @return 最终编码生成的图片文件数据
   */
  virtual std::shared_ptr<TData> FinishEncoding() = 0;

  /**
   * @brief 创建一个默认的图片编码器实例
   * @param format 图片格式
   * @param options 控制图片编码算法的各配置参量
   * @return 图片编码器
   */
  static std::shared_ptr<ImageEncoder> Make(ImageCodecFormat format, const Options &options = Options::MakeDefault());

  /**
   * @brief 【快捷方法】使用默认的图片编码器，将ImageFrame编码生成单帧图片文件数据
   * @param image_frame 待编码的图片帧数据
   * @param format 图片格式
   * @param compression_quality 编码压缩质量，即Options::compression_quality
   * @return 最终编码生成的图片文件数据
   */
  static std::shared_ptr<TData> Encode(const std::shared_ptr<ImageFrame> &image_frame, ImageCodecFormat format,
                                       float compression_quality = 1.0);

  /**
   * @brief 【快捷方法】使用默认的图片编码器，将Bitmap编码生成单帧图片文件数据
   * @param bitmap 待编码的图片像素数据
   * @param format 图片格式
   * @param compression_quality 编码压缩质量，即Options::compression_quality
   * @return 最终编码生成的图片文件数据
   */
  static inline std::shared_ptr<TData> Encode(const std::shared_ptr<Bitmap> &bitmap, ImageCodecFormat format,
                                              float compression_quality = 1.0) {
    assert(bitmap);
    return Encode(TDF_MAKE_SHARED(BitmapImageFrame, bitmap), format, compression_quality);
  }

  /**
   * @brief 【快捷方法】使用默认的图片编码器，将Texture编码生成单帧图片文件数据
   * @param texture 待编码的图片纹理
   * @param format 图片格式
   * @param compression_quality 编码压缩质量，即Options::compression_quality
   * @return 最终编码生成的图片文件数据
   */
  static inline std::shared_ptr<TData> Encode(const std::shared_ptr<Texture> &texture, ImageCodecFormat format,
                                              float compression_quality = 1.0) {
    assert(texture);
    return Encode(TDF_MAKE_SHARED(TextureImageFrame, texture), format, compression_quality);
  }

 protected:
  ImageCodecFormat format_;
  Options options_;
};

}  // namespace tdfcore
