//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/image_info.h"
#include "core/common/object.h"
#include "core/engine/graphics/texture.h"

namespace tdfcore {

class Bitmap : public Object, public std::enable_shared_from_this<Bitmap> {
 public:
  ~Bitmap() override;
  Bitmap(const Bitmap &) = delete;
  Bitmap &operator=(const Bitmap &) = delete;

  /**
   * @brief 构造函数
   * @details 本类将根据info参数，自动分配并管理像素缓冲区
   * @param info 位图基本信息
   */
  explicit Bitmap(const ImageInfo &info);

  /**
   * @brief 释放像素缓冲区的回调函数
   * @param pixels 像素缓冲区
   * @param context 与像素缓冲区相关联的环境上下文
   */
  using ReleaseCallback = std::function<void(void *pixels, void *context)>;

  /**
   * @brief 构造函数
   * @details 由调用方负责分配像素缓冲区，并移交管理权至本类；当本类析构时，将通过调用方指定的释放函数，以释放像素缓冲区
   * @param info 位图基本信息
   * @param pixels 像素缓冲区
   * @param release_callback 用于释放pixels像素缓冲区的回调函数（在本类析构时会发出该回调），不可为空
   * @param context 与像素缓冲区相关联的环境上下文，将透传给release_callback
   */
  Bitmap(const ImageInfo &info, void *pixels, const ReleaseCallback &release_callback, void *context = nullptr);

  /**
   * @brief 获取基本信息
   */
  const ImageInfo &Info() const { return info_; }

  /**
   * @brief 获取宽度
   */
  int32_t Width() const { return info_.Width(); }

  /**
   * @brief 获取高度
   */
  int32_t Height() const { return info_.Height(); }

  /**
   * @brief 获取色彩模式
   */
  ColorType GetColorType() const { return info_.GetColorType(); }

  /**
   * @brief 获取透明模式
   */
  AlphaType GetAlphaType() const { return info_.GetAlphaType(); }

  /**
   * @brief 获取内部的像素缓冲区
   */
  void *Pixels() const { return pixels_; }

  /**
   * @brief 读取像素
   * @param dst_info 目标图像信息
   * @param dst_pixels 目标像素缓冲区
   * @param src_x X向的读取起始点
   * @param src_y Y向的读取起始点
   */
  bool ReadPixels(const ImageInfo &dst_info, void *dst_pixels, int src_x = 0, int src_y = 0) const;

  /**
   * @brief 写入像素
   * @param src_info 源图像信息
   * @param src_pixels 源像素缓冲区
   * @param dst_x X向的写入起始点
   * @param dst_y Y向的写入起始点
   */
  bool WritePixels(const ImageInfo &src_info, const void *src_pixels, int dst_x = 0, int dst_y = 0);

  /**
   * @brief 创建Texture
   */
  std::shared_ptr<Texture> MakeTexture(std::shared_ptr<Device> device) const;

  /**
   * @brief 读取Texture并创建一个Bitmap
   */
  static std::shared_ptr<Bitmap> MakeFromTexture(const std::shared_ptr<Texture> &texture);

 private:
  ImageInfo info_;
  void *pixels_ = nullptr;
  ReleaseCallback release_callback_;
  void *context_ = nullptr;
  struct Utility;
  Utility *utility_;
};

}  // namespace tdfcore
