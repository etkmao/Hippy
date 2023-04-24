//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include "core/common/callback_notifier.h"
#include "core/common/object.h"
#include "core/common/point.h"
#include "core/common/size.h"
#include "core/engine/graphics/canvas.h"
#include "core/engine/raster/raster_cache.h"

namespace tdfcore {

/**
 * @brief 画笔（抽象接口类）
 * @details 需要子类化，在子类中实现自定义绘制操作
 */
class CustomPainter : public Object {
 public:
  class CacheInfo {
   public:
    const RasterCacheResult last_frame_cache_result;
    bool should_cache;
    std::any extra_info;
  };
  using CustomPaintFunction = std::function<void(Canvas *canvas, TSize size, CacheInfo *info)>;
  static constexpr const char *kCacheExtraInfoKey = "cache_extra_info";
  /**
   * @brief 绘制操作（子类必须重写该方法）
   * @details 子类重写该方法，实现具体的绘制操作
   * @param canvas 画布
   * @param size 画布尺寸
   * @param cache_texture 前一次绘制的缓存
   * @param cache_extra_info 前一次绘制的缓存额外信息
   * @return 返回绘制缓存的Texture的额外信息
   */
  CacheInfo Paint(Canvas *canvas, TSize size) const {
    CacheInfo info{cache_result_, raster_cache_enabled_, std::any()};
    if (paint_function_) {
      paint_function_(canvas, size, &info);
    }
    return info;
  }

  void SetSize(const TSize &size) { size_ = size; }

  const TSize &GetSize() const { return size_; }

  /**
   * @brief 设置绘制操作
   * @param paint_function 绘制操作
   */
  void SetPaintFunction(const CustomPaintFunction &paint_function) { paint_function_ = paint_function; }

  /**
   * @brief 获取绘制操作
   */
  const CustomPaintFunction &GetPaintFunction() const { return paint_function_; }

  void SetRasterCacheEnabled(bool enabled) { raster_cache_enabled_ = enabled; }

  bool GetRasterCacheEnabled() const { return raster_cache_enabled_; }

  void SetCacheResult(RasterCacheResult result) { cache_result_ = result; }

  const RasterCacheResult &GetCacheResult() const { return cache_result_; }

 protected:
  explicit CustomPainter(const CustomPaintFunction &paint_function = nullptr) : paint_function_(paint_function) {}
  TSize size_;
  bool raster_cache_enabled_ = false;
  RasterCacheResult cache_result_;
  CustomPaintFunction paint_function_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
