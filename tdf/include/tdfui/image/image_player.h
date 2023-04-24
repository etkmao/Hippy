//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <list>
#include <unordered_map>
#include "core/common/image.h"
#include "core/support/animation/ticker.h"
#include "tdfui/image/image_manager.h"

namespace tdfcore {

/**
 * @brief 动图播放器
 */
class ImagePlayer : public Object, public std::enable_shared_from_this<ImagePlayer> {
 public:
  ~ImagePlayer() override;

  /**
   * @brief 构造函数
   * @param image 动图
   * @param manager ImageManager实例，用于加载图片数据
   */
  ImagePlayer(const std::shared_ptr<Image> &image, const std::shared_ptr<ImageManager> &manager);

  /**
   * @brief 初始化
   */
  void Init() override;

  /**
   * @brief 设置图片加载选项
   * @param scale_options 缩放配置
   * @param processors 图像后置处理操作
   */
  void SetImageOptions(const ImageScaleOptions &scale_options,
                       const std::vector<std::shared_ptr<ImageProcessor>> &processors);

  /**
   * @brief 播放输出回调函数
   * @param image_frame 图片帧
   */
  using OutputCallback = std::function<void(const ImageData::Frame &image_frame)>;

  /**
   * @brief 设置播放输出回调函数
   */
  void SetOutputCallback(const OutputCallback &callback) { output_callback_ = callback; }

  /**
   * @brief 播放结束回调函数
   */
  using PlayToEndCallback = std::function<void()>;

  /**
   * @brief 设置播放结束回调函数
   */
  void SetPlayToEndCallback(const PlayToEndCallback &callback) { play_to_end_callback_ = callback; }

  /**
   * @brief 错误通知回调函数
   */
  using ErrorCallback = std::function<void(uint64_t error_frame_index)>;

  /**
   * @brief 设置错误通知回调函数
   */
  void SetErrorCallback(const ErrorCallback &callback) { error_callback_ = callback; }

  /**
   * @brief 播放
   */
  void Play();

  /**
   * @brief 暂停
   */
  void Pause();

  /**
   * @brief 跳至指定帧
   * @param frame_index 帧索引
   */
  void Seek(uint64_t frame_index);

  /**
   * @brief 是否正在播放
   */
  bool IsPlaying() const { return is_playing_; }

  /**
   * @brief 获取播放器的当前位置（帧索引）
   */
  uint64_t GetCurrentFrameIndex() const { return current_frame_index_; }

  /**
   * @brief 获取帧总数
   */
  uint64_t GetFrameCount() const { return frame_count_; }

 private:
  static constexpr const int64_t kPreloadFrameCount = 1;
  std::shared_ptr<Image> image_;
  std::shared_ptr<ImageManager> manager_;
  ImageOptions image_options_ = ImageOptions();
  OutputCallback output_callback_;
  PlayToEndCallback play_to_end_callback_;
  ErrorCallback error_callback_;
  bool is_playing_ = false;
  uint64_t frame_count_ = 0;
  uint64_t current_frame_index_ = 0;
  TDuration current_frame_duration_;
  TDuration current_frame_playing_time_;
  TimePoint last_timestamp_;
  std::shared_ptr<Ticker> ticker_;
  std::shared_ptr<ImageLoadTask> load_task_;
  std::list<uint64_t> loading_frame_indexes_;
  std::unordered_map<uint64_t, ImageData::Frame> frames_;

  void PlayForward();
  void PlayFrame(const ImageData::Frame &frame, const TDuration &playing_time);
  void SetCurrentFrame(uint64_t frame_index, const TDuration &playing_time);
  void PrepareFrames();
  void LoadNextFrame();
  void LoadFrame(uint64_t index, const std::function<void(const std::shared_ptr<ImageData> &)> &callback);
  void Clear();

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
