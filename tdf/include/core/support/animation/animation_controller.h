//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <utility>
#include "core/common/value_change_notifier.h"
#include "core/support/animation/animation.h"
#include "core/support/animation/ticker.h"

namespace tdfcore {

enum class AnimationDirection {
  kForward,
  kReverse,
};

class AnimationControllerStatusNotifier : public ValueNotifier<bool> {
 public:
  AnimationControllerStatusNotifier() : ValueNotifier<bool>(false) {}
};

class AnimationController : public Object,
                                    public ValueNotifier<double>,
                                    public std::enable_shared_from_this<AnimationController> {
 public:
  [[maybe_unused]] static const int32_t kRepeatForever = -1;

  ~AnimationController() override = default;

  /**
   * @brief 添加动画
   * @param animation 动画
   */
  void AddAnimation(std::shared_ptr<BaseAnimation> animation);

  /**
   * @brief 移除动画
   * @param animation 动画
   */
  void RemoveAnimation(const std::shared_ptr<BaseAnimation> &animation);

  /**
   * @brief 获取动画数组
   * @param return 数组
   */
  constexpr const std::vector<std::shared_ptr<BaseAnimation>> &GetAnimationList() const { return animations_; }

  /**
   * @brief 设置动画延迟启动时间
   * @param delay 动画延迟启动时间，单位 ns，默认 0 ns
   */
  void SetDelay(TDuration delay) { delay_ = delay; }

  /**
   * @brief 设置动画重复执行次数
   * @param repeat_count 动画重复执行次数，-1 表示无限重复，正值表示确定次数，默认 0
   */
  void SetRepeatCount(int32_t repeat_count);

  /**
   * @brief 设置动画方向
   * @param direction 动画方向
   */
  void SetDirection(AnimationDirection direction);

  /**
   * @brief 设置动画进度
   * @param percent 动画进度值0.0~1.0
   */
  void SetProgress(double percent);

  /**
   * @brief 获取方向
   * @param return 方向
   */
  constexpr AnimationDirection GetDirection() const { return direction_; }

  /**
   * @brief 添加状态监听
   * @param listeners
   */
  void AddStatusListener(Callback listener) { status_notifier_.AddListener(std::move(listener)); }

  /**
   * @brief 根据进度播放动画
   */
  void Play();

  /**
   * @brief 暂停动画
   */
  void Pause();

 protected:
  explicit AnimationController(TDuration duration, const std::shared_ptr<BaseAnimation> &animation = nullptr);
  void Init() override;
  void SetValueInternal(double value);
  void CheckStatusChanged();
  void OnEnd();
  void UpdateRemain();

 private:
  TDuration duration_ = TDuration(0);
  AnimationDirection direction_ = AnimationDirection::kForward;
  std::shared_ptr<Ticker> ticker_;
  double lower_bound_ = 0.0;
  double upper_bound_ = 1.0;
  std::vector<std::shared_ptr<BaseAnimation>> animations_;
  AnimationControllerStatusNotifier status_notifier_;
  double begin_percent_ = 0.0;
  TDuration remain_duration_ = duration_;
  TDuration delay_ = TDuration(0);
  int32_t repeat_count_ = 0;
  int32_t remain_repeat_count_ = repeat_count_;
  bool is_animating_ = false;
  bool is_paused_ = true;

  void SetPause(bool is_paused);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
