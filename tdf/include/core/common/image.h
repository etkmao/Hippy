//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <string>
#include <variant>
#include <vector>
#include "core/common/image_frame.h"

namespace tdfcore {

/**
 * @brief 图片资源
 */
class Image : public Object {
 public:
  ~Image() override = default;

  /**
   * @brief 创建图片时采用的数据来源
   */
  enum class DataSource : uint8_t {
    kUrl = 0,
    kRawData = 1,
    kImageFrame = 2,
  };

  /**
   * @brief 【构造函数】图片URL
   */
  explicit Image(const std::string &url);

  /**
   * @brief 【构造函数】未解码的原始数据
   */
  explicit Image(const std::shared_ptr<TData> &raw_data);

  /**
   * @brief 【构造函数】图片帧序列
   */
  explicit Image(const std::vector<std::shared_ptr<ImageFrame>> &image_frames);

  /**
   * @brief 【构造函数】【快捷方法】单帧图
   */
  explicit Image(const std::shared_ptr<Bitmap> &bitmap);

  /**
   * @brief 【构造函数】【快捷方法】多帧图
   * @param frame_duration 单帧时长
   */
  Image(const std::vector<std::shared_ptr<Bitmap>> &bitmaps, const TDuration &frame_duration);

  /**
   * @brief 【构造函数】【快捷方法】单帧图
   */
  explicit Image(const std::shared_ptr<Texture> &texture);

  /**
   * @brief 【构造函数】【快捷方法】多帧图
   * @param frame_duration 单帧时长
   */
  Image(const std::vector<std::shared_ptr<Texture>> &textures, const TDuration &frame_duration);

  /**
   * @brief 创建图片时采用的数据类型
   */
  DataSource GetDataSource() const { return static_cast<DataSource>(data_.index()); }

  /**
   * @brief 获取图片创建时的URL；如果本Image不是通过URL的方式创建的，则返回空字符串
   */
  std::string GetUrl() const;

  /**
   * @brief 获取图片创建时的未解码原始数据；如果本Image不是通过未解码原始数据的方式创建的，则返回nullptr
   */
  std::shared_ptr<TData> GetRawData() const;

  /**
   * @brief 获取图片创建时的ImageFrame序列；如果本Image不是通过ImageFrame的方式创建的，则返回空数组
   */
  std::vector<std::shared_ptr<ImageFrame>> GetImageFrames() const;

  /**
   * @brief 是否为相同的图片
   */
  bool IsEqual(const std::shared_ptr<Image> &other) const;

 private:
  // 注意：variant内的类型顺序需与DataSource枚举顺序一致
  using Data = std::variant<std::string, std::shared_ptr<TData>, std::vector<std::shared_ptr<ImageFrame>>>;
  Data data_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
