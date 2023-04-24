//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/image.h"
#include "core/common/size.h"
#include "core/engine/render/image_object.h"
#include "core/tdfi/timer.h"
#include "tdfui/image/image_manager.h"
#include "tdfui/image/image_player.h"
#include "tdfui/view/view.h"

namespace tdfcore {

class ImageView : public View {
  TDF_REFF_CLASS_META(ImageView)

 public:
  ~ImageView() override = default;

  /**
   * @brief 设置缩放类型
   * @param scaleType 缩放类型
   */
  void SetScaleType(ScaleType scale_type);

  /**
   * @brief 获取缩放类型
   */
  ScaleType GetScaleType() const;

  /**
   * @brief 设置图片
   * @param image 图片
   */
  void SetImage(const std::shared_ptr<Image> &image);

  /**
   * @brief 获取已设置的图片
   */
  std::shared_ptr<Image> GetImage() const { return primary_item_.image; }

  /**
   * @brief 设置占位图
   * @details 当Image为空或Image正在加载数据（尚不能显示图像）期间，ImageView将展示占位图
   * @param placeholder 占位图
   */
  void SetPlaceholder(const std::shared_ptr<Image> &placeholder);

  /**
   * @brief 获取已设置的占位图
   */
  std::shared_ptr<Image> GetPlaceholder() const { return placeholder_item_.image; }

  /**
   * @brief 开始播放动图，对非动图无效
   */
  void StartAnimation();

  /**
   *@brief 停止播放动图，对非动图无效
   */
  void StopAnimation();

  /**
   * @brief 判断动图是否正在播放，非动图返回false
   */
  bool IsAnimating() const;

  /**
   * @brief 设置动图循环次数
   */
  void SetLoopCount(int32_t loop_count) { total_loop_count_ = loop_count; }

  /**
   * @brief 获得动图循环次数，默认为0，代表一直循环
   * @return 动图循环次数
   */
  int32_t GetLoopCount() const { return total_loop_count_; }

  /**
   * @brief 设置动图是否自动播放动图
   */
  void SetAutoPlay(bool is_auto_play) { is_auto_play_ = is_auto_play; }

  /**
   * @brief 获得动图是否自动播放，默认为true，非动图此属性无意义
   * @return 是否自动播放
   */
  bool GetAutoPlay() const { return is_auto_play_; }

  /**
   * @brief 是否预渲染，默认false
   */
  void SetPreRasterEnabled(bool pre_raster_enabled) { pre_raster_enabled_ = pre_raster_enabled; }

  /**
   * @brief 获取预渲染
   */
  bool GetPreRasterEnabled() const { return pre_raster_enabled_; }

  /**
   * @brief 图片加载开始回调
   */
  using ImageLoadStartCallback = std::function<void()>;

  /**
   * @brief 设置图片加载开始回调
   */
  void SetImageLoadStartCallback(const ImageLoadStartCallback &callback) {
    primary_item_.load_start_callback = callback;
  }

  /**
   * @brief 图片加载完成回调
   * @param error 错误
   */
  using ImageLoadFinishCallback = std::function<void(ImageError error)>;

  /**
   * @brief 设置图片加载完成回调
   */
  void SetImageLoadFinishCallback(const ImageLoadFinishCallback &callback) {
    primary_item_.load_finish_callback = callback;
  }

  /**
   * @brief 图片加载进度回调
   * @param progress 进度，值区间[0.0 ~ 1.0]
   */
  using ImageLoadProgressCallback = std::function<void(float progress)>;

  /**
   * @brief 设置图片加载进度回调
   */
  void SetImageLoadProgressCallback(const ImageLoadProgressCallback &callback) {
    primary_item_.load_progress_callback = callback;
  }

 protected:
  enum RenderObjectIndex { kImage = View::kRenderObjectCount, kRenderObjectCount };

  explicit ImageView(const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent());
  void Init() override;
  void Layout() override;
  void Mount() override;
  void Unmount() override;
  int32_t GetTotalRenderObjectCount() override;
  void UpdateRenderObject(const std::shared_ptr<Texture> &texture);

 private:
  struct ImageItem {
    struct State {
      std::shared_ptr<ImageData> data;
      std::shared_ptr<ImageLoadTask> task;
      std::shared_ptr<ImagePlayer> player;
      bool needs_notify_load_status = true;
      bool needs_load_data = false;
      bool is_pre_rastered = false;
      int32_t current_looped_count = 0;
    };

    std::shared_ptr<State> state = std::make_shared<State>();
    std::shared_ptr<Image> image;
    ImageLoadStartCallback load_start_callback;
    ImageLoadFinishCallback load_finish_callback;
    ImageLoadProgressCallback load_progress_callback;

    void ResetState();
    inline bool IsValid() const { return image != nullptr; }
    inline bool HasData() const { return state && state->data; }
    inline bool CanAnimate() const { return HasData() && (state->data->GetImageInfo().total_frame_count > 1); }
    inline bool IsAnimating() const { return (state && state->player) ? state->player->IsPlaying() : false; }
  };

  bool pre_raster_enabled_ = false;
  bool is_auto_play_ = true;
  int32_t total_loop_count_ = 0;
  ImageItem primary_item_;
  ImageItem placeholder_item_;
  ImageItem *current_animating_item_ = nullptr;

  void Refresh();
  void Display();
  void ReplaceImage(ImageItem &item, const std::shared_ptr<Image> &image);
  void LoadDataIfNeeded(ImageItem &item);
  void LoadData(ImageItem &item, const ImageOptions &options);
  ImageItem *GetFirstValidImageItem();
  ImageItem *GetFirstDisplayableImageItem();
  ImageOptions MakeImageOptions();

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::ImageView, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::ImageView)
