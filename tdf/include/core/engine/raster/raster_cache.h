//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <cassert>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "core/common/m33.h"
#include "core/common/macro.h"
#include "core/engine/graphics/texture.h"
#include "core/engine/graphics/device.h"
#include "core/engine/graphics/paint.h"
#include "core/engine/graphics/picture.h"

namespace tdfcore {

class Layer;
class Canvas;

/**
 * 纹理缓存策略
 */
enum class RasterCachePolicy {
  // 没有指定具体策略，跟随祖先节点策略
  kInherited,
  // 匹配id和矩阵（包含旋转和缩放）
  kStrict,
  // 只匹配id
  kLoose,
};

/**
 * @brief   纹理缓存key，由`id`和`matrix`组成，`ignore_matrix_`为true时则在查找时只对比`id`
 */
template <typename ID>
class RasterCacheKey {
 public:
  RasterCacheKey(ID id, const TM33 &ctm) : id_(id), matrix_(ctm) {
    matrix_[TM33::TRANS_X] = 0;
    matrix_[TM33::TRANS_Y] = 0;
  }

  /**
   * @brief 获取缓存id
   */
  ID GetId() const { return id_; }

  /**
   * @brief 获取缓存对应的矩阵
   */
  const TM33 &GetMatrix() const { return matrix_; }

  /**
   * @brief 设置查找时是否忽略矩阵比较，若忽略则只比较id
   */
  void SetIgnoreMatrix(bool ignored) { ignore_matrix_ = ignored; }

  struct Hash {
    uint32_t operator()(RasterCacheKey const &key) const { return static_cast<uint32_t>(std::hash<ID>()(key.id_)); }
  };

  struct Equal {
    constexpr bool operator()(const RasterCacheKey &lhs, const RasterCacheKey &rhs) const {
      return (lhs.ignore_matrix_ || rhs.ignore_matrix_) ? lhs.id_ == rhs.id_
                                                        : lhs.id_ == rhs.id_ && lhs.matrix_ == rhs.matrix_;
    }
  };

  template <class Value>
  using Map = std::unordered_map<RasterCacheKey, Value, Hash, Equal>;

 private:
  ID id_;
  TM33 matrix_;
  // 只用于查找是传入的参数，储存在map中不要修改
  bool ignore_matrix_ = false;
};

// uint32_t picture uniqueID
using PictureRasterCacheKey = RasterCacheKey<uint32_t>;

// uint64_t layer unique_id
using LayerRasterCacheKey = RasterCacheKey<uint64_t>;

/**
 * @brief   纹理缓存结构，一般用于保存将SkPicture/Layer纹理后的SkImage
 */
class RasterCacheResult {
 public:
  RasterCacheResult() = default;
  RasterCacheResult(const RasterCacheResult &other) = default;
  RasterCacheResult(std::shared_ptr<Texture> texture, const TRect &logical_rect,
                    std::optional<TM33> loose_cache_matrix = std::optional<TM33>());

  /**
   * @brief 根据期望的绘制矩阵计算出宽松缓存策略时对应的绘制矩阵
   * @param ctm 期望的绘制矩阵
   * @return    成功返回true，失败返回false
   */
  bool AssureLooseDrawMatrix(const TM33 &ctm);

  /**
   * 判断缓存是否合法
   */
  operator bool() const { return static_cast<bool>(texture_); }
  bool IsValid() const { return static_cast<bool>(texture_); }

  /**
   * @brief 获取本地绘制区域
   */
  const TRect &GetLogicalRect() const { return logical_rect_; }

  /**
   * @brief 获取缓存的纹理图像
   */
  const std::shared_ptr<Texture> &GetImage() const { return texture_; }

  /**
   * @brief 获取宽松纹理策略下的绘制矩阵
   */
  std::optional<std::pair<TM33, TM33>> GetLooseDrawMatrix() const { return loose_cache_draw_matrix_; }

  /**
   * @brief 获取缓存生成时对应的纹理矩阵，用于计算宽松纹理策略绘制矩阵
   */
  std::optional<TM33> GetLooseCacheMatrix() const { return loose_cache_matrix_; }

  /**
   * @brief 获取纹理图像的像素尺寸
   */
  ISize ImageDimensions() const { return texture_ ? texture_->Dimensions() : ISize::Make(0, 0); }

  void SetExtraInfo(const std::any &info) { extra_info_ = info; }

  const std::any &GetExtraInfo() const { return extra_info_; }

 private:
  std::shared_ptr<Texture> texture_;
  TRect logical_rect_;
  std::optional<TM33> loose_cache_matrix_;
  std::optional<std::pair<TM33, TM33>> loose_cache_draw_matrix_;
  std::any extra_info_;
};

struct PrerollContext;

/**
 * @brief   纹理缓存管理器，负责生成、管理RasterCacheResult
 */
class RasterCache {
 public:
  // 每次上屏时可以生成的最大缓存个数，一次生成太多缓存会引起掉帧
  static constexpr int kDefaultPictureCacheLimitPerFrame = 3;

  /**
   * @brief 构造函数
   * @param access_threshold 访问阈值，当一个SkPicture被访问次数超过此阈值时才有可能缓存
   * @param picture_cache_limit_per_frame
   */
  explicit RasterCache(size_t access_threshold = 3,
                       size_t picture_cache_limit_per_frame = kDefaultPictureCacheLimitPerFrame);

  static RasterCacheResult Rasterize(const std::shared_ptr<Device> &device, const TM33 &ctm, bool checkerboard,
                                     const TRect &logical_rect, const std::function<void(Canvas *)> &draw_function);

  static RasterCacheResult RasterizePicture(const std::shared_ptr<Device> &device, std::shared_ptr<Picture> picture,
                                            const TM33 &ctm, bool is_checkerboard_enabled);

  /**
   * @brief 将矩阵应用到矩形上，并返回结果
   */
  static TRect GetDeviceBounds(const TRect &rect, const TM33 &ctm) {
    TRect device_rect;
    ctm.MapRect(&device_rect, rect);
    TRect bounds;
    device_rect.RoundOut(&bounds);
    return bounds;
  }

  /**
   * @brief 四舍五入矩阵偏移
   */
  static TM33 GetIntegralTransCTM(const TM33 &ctm) {
    TM33 result = ctm;
    result[TM33::TRANS_X] = TScalarRoundToScalar(ctm.GetTranslateX());
    result[TM33::TRANS_Y] = TScalarRoundToScalar(ctm.GetTranslateY());
    return result;
  }

  /**
   * @brief 生成指定SkPicture的缓存
   * @return    成功返回true
   */
  bool Prepare(const std::shared_ptr<Device> &device, const std::shared_ptr<Picture> &picture,
               const TM33 &transformation_matrix, bool is_complex, bool will_change, RasterCachePolicy policy);

  /**
   * @brief 生成指定Layer的缓存
   * @return    成功返回true
   */
  void Prepare(PrerollContext &context, std::shared_ptr<Layer> layer, const TM33 &ctm, RasterCachePolicy policy);

  /**
   * @brief 获取指定SkPicture对应的缓存
   */
  RasterCacheResult Get(const Picture &picture, const TM33 &ctm,
                        RasterCachePolicy policy = RasterCachePolicy::kStrict) const;

  /**
   * @brief 获取指定Layer对应的缓存
   */
  RasterCacheResult Get(const std::shared_ptr<Layer> &layer, const TM33 &ctm,
                        RasterCachePolicy policy = RasterCachePolicy::kStrict) const;

  /**
   * @brief 绘制缓存
   */
  static void DrawResult(const RasterCacheResult &result, Canvas *canvas,
                         RasterCachePolicy policy = RasterCachePolicy::kStrict, TPaint *paint = nullptr);

  /**
   * @brief 查找并绘制SkPicture对应的缓存
   * @return    成功绘制返回true
   */
  bool DrawIfFound(const Picture &picture, Canvas *canvas, TPaint *paint = nullptr,
                   RasterCachePolicy policy = RasterCachePolicy::kStrict) const;

  /**
   * @brief 查找并绘制Layer对应的缓存
   * @return    成功绘制返回true
   */
  bool DrawIfFound(const std::shared_ptr<Layer> &layer, Canvas *canvas, TPaint *paint = nullptr,
                   RasterCachePolicy policy = RasterCachePolicy::kStrict) const;

  /**
   * @brief 清理当前帧没有用到的缓存
   */
  void SweepAfterFrame();

  /**
   * @brief 清理所有缓存
   */
  void Clear();

  /**
   * @brief 是否在缓存图像上绘制checkboard效果
   */
  void SetCheckboardCacheImages(bool checkerboard);

  /**
   * @brief 获取当前缓存数
   */
  size_t GetCachedEntriesCount() const { return layer_cache_.size() + picture_cache_.size(); }

 private:
  struct Entry {
    bool is_used_in_this_frame = false;
    size_t access_count = 0;
    RasterCacheResult image;
  };

  template <class Cache, typename Key>
  void UpdateLooseCacheEntryKey(Cache &c, Key k) {
    k.SetIgnoreMatrix(true);
    auto found = c.find(k);
    assert(found != c.end());
    if (found->first.GetMatrix() == k.GetMatrix()) {
      return;
    }
    auto entry = found->second;
    c.erase(found);
    k.SetIgnoreMatrix(false);
    c.insert({k, entry});
  }

  template <class Cache>
  static void SweepOneCacheAfterFrame(Cache &cache) {
    std::vector<typename Cache::iterator> dead;

    for (auto it = cache.begin(); it != cache.end(); ++it) {
      Entry &entry = it->second;
      if (!entry.is_used_in_this_frame) {
        dead.push_back(it);
      }
      entry.is_used_in_this_frame = false;
    }

    for (auto it : dead) {
      cache.erase(it);
    }
  }

  const size_t access_threshold_;
  const size_t picture_cache_limit_per_frame_;
  size_t picture_cached_this_frame_ = 0;
  mutable PictureRasterCacheKey::Map<Entry> picture_cache_;
  mutable LayerRasterCacheKey::Map<Entry> layer_cache_;
  bool is_checkerboard_images_enabled_;

  Entry &FindOrCreate(const std::shared_ptr<Picture> &picture, const TM33 &transformation_matrix,
                      RasterCachePolicy policy);
  Entry &FindOrCreate(const std::shared_ptr<Layer> &picture, const TM33 &transformation_matrix,
                      RasterCachePolicy policy);
};

}  // namespace tdfcore
