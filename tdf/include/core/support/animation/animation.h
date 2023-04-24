//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <vector>
#include "core/common/object.h"
#include "core/common/time.h"
#include "core/common/value_change_notifier.h"
#include "core/support/animation/curves.h"
#include "core/support/animation/tween.h"

namespace tdfcore {

class AnimationStatusNotifier : public ValueNotifier<bool> {
 public:
  AnimationStatusNotifier() : ValueNotifier<bool>(false) {}
};

/**
 * @brief 动画基类
 */
class BaseAnimation : public Object {
 public:
  BaseAnimation() : status_notifier_(AnimationStatusNotifier()) {}

  /**
   * @brief 根据进度更新动画具体值
   * @param percent 动画进度因子 0~1
   */
  virtual void UpdateValue(double percent) = 0;

  /**
   * @brief 设置动画差权重，默认1.0
   */
  void SetWeight(double weight) { weight_ = weight; }

  /**
   * @brief 获取动画权重
   */
  constexpr double GetWeight() const { return weight_; }

  /**
   * @brief 设置动画暂停状态
   */
  void SetPause(bool is_paused) { is_paused_ = is_paused; }

  /**
   * @brief 添加状态监听
   * @param listeners
   */
  void AddStatusListener(AnimationStatusNotifier::Callback listener) {
    status_notifier_.AddListener(std::move(listener));
  }

 protected:
  bool is_animating_ = false;
  double weight_ = 1.0;
  bool is_paused_ = true;
  AnimationStatusNotifier status_notifier_;
};

/**
 * @brief 动画模板类
 * @tparam T
 */
template <typename T>
class Animation : public BaseAnimation, public ValueNotifier<T> {
 public:
  ~Animation() override = default;

  /**
   * @brief 设置动画差值器
   * @param tween 动画差值器
   */
  void SetTween(std::shared_ptr<Tween<T>> tween) { tween_ = std::move(tween); }

  void UpdateValue(double percent) override {
    assert(percent >= 0.0 && percent <= 1.0);
    auto value = tween_->Lerp(percent);
    if (value == this->GetValue()) {
      this->NotifyListeners();
    } else {
      this->SetValue(value);
    }
    if (percent <= 0.0 || percent >= 1.0) {
      is_animating_ = false;
    } else {
      is_animating_ = !is_paused_;
    }
    status_notifier_.SetValue(is_animating_);
  }

  constexpr const T& GetBegin() const { return tween_->GetBegin(); }
  constexpr const T& GetEnd() const { return tween_->GetEnd(); }

 protected:
  Animation(T begin, T end) : ValueNotifier<T>(begin), tween_(TDF_MAKE_SHARED(Tween<T>, begin, end)) {}

  explicit Animation(std::shared_ptr<Tween<T>> tween) : ValueNotifier<T>(tween->GetBegin()) {
    tween_ = std::move(tween);
  }

  std::shared_ptr<Tween<T>> tween_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
