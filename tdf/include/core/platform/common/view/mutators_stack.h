//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <utility>
#include <variant>
#include <vector>
#include "core/common/m33.h"
#include "core/common/object.h"
#include "core/common/rect.h"
#include "core/common/rrect.h"
#include "core/engine/graphics/path.h"

namespace tdfcore {

/**
 * @brief 变换因子类型
 */
enum MutatorType { kClipRect, kClipRRect, kTransform, kOpacity, kClipPath };

/**
 * @brief 变换因子
 * 变换、裁剪、透明度都可以是一个Mutator
 */
class Mutator : public Object {
 public:
  Mutator(const Mutator& other);
  explicit Mutator(const TRect& rect) : content_(rect) {}
  explicit Mutator(const RRect& rrect) : content_(rrect) {}
  explicit Mutator(const Path& path) : content_(path) {}
  explicit Mutator(const TM33& matrix) : content_(matrix) {}
  explicit Mutator(const int32_t& alpha) : content_(alpha) {}

  ~Mutator() override {}

  TRect GetRect() const { return std::get<TRect>(content_); }

  RRect GetRRect() const { return std::get<RRect>(content_); }

  Path GetPath() const { return std::get<Path>(content_); }

  TM33 GetMatrix() const { return std::get<TM33>(content_); }

  int32_t GetAlpha() const { return std::get<int32_t>(content_); }

  float GetAlphaFloat() const { return std::get<int32_t>(content_) / 255.0; }

  constexpr MutatorType GetType() const { return static_cast<MutatorType>(content_.index()); }

 private:
  std::variant<TRect, RRect, TM33, int32_t, Path> content_;
};

/**
 * @brief 变化因子栈
 * 主要记录`LayerTree`变换、裁剪、透明度等操作
 */
class MutatorsStack {
 public:
  void PushClipRect(const TRect& rect);
  void PushClipRRect(const RRect& rrect);
  void PushClipPath(const Path& path);
  void PushTransform(const TM33& matrix);
  void PushOpacity(const int32_t& opacity);

  void Pop();

  constexpr const std::vector<std::shared_ptr<Mutator>>& GetStack() const { return vector_; }

 private:
  std::vector<std::shared_ptr<Mutator>> vector_;
};
}  // namespace tdfcore
