//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <vector>
#include "core/support/animation/animation.h"

namespace tdfcore {

/**
 * @brief 动画集合基类
 * @tparam T
 */
class AnimationSet : public BaseAnimation {
 public:
  ~AnimationSet() override = default;
  explicit AnimationSet(const std::vector<std::shared_ptr<BaseAnimation>> &animations) : animations_(animations) {
    assert(!animations.empty());
    animations_.assign(animations.begin(), animations.end());
  }

  /**
   * @brief 获取集合里的动画组
   * @return 动画数组
   */
  constexpr const std::vector<std::shared_ptr<BaseAnimation>> &GetChildren() const { return animations_; }

  /**
   * @brief 获取集合权重总值
   * @return 权重总值
   */
  constexpr double GetTotalWeight() const { return total_weights_; }

 protected:
  std::vector<std::shared_ptr<BaseAnimation>> animations_;
  double total_weights_ = 0.0;

  void EvaluateAt(double animation_percent, int index) {
    assert(animation_percent >= 0.0 && animation_percent <= 1.0);
    assert(!animations_.empty());
    auto item = animations_[index];
    item->UpdateValue(animation_percent);
  }
};

/**
 * @brief 串行动画集合
 */
class SerialAnimationSet : public AnimationSet {
 public:
  ~SerialAnimationSet() override = default;
  explicit SerialAnimationSet(const std::vector<std::shared_ptr<BaseAnimation>> &animations);
  void UpdateValue(double percent) override;
};

/**
 * @brief 并行动画集合
 */
class ParallelAnimationSet : public AnimationSet {
 public:
  ~ParallelAnimationSet() override = default;
  explicit ParallelAnimationSet(const std::vector<std::shared_ptr<BaseAnimation>> &animations);
  void UpdateValue(double percent) override;
};

}  // namespace tdfcore
