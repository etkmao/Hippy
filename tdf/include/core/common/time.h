//
//
// Copyright (c) Tencent Corporation. All rights reserved.
//
//
#pragma once

#include <chrono>
#include "core/common/object.h"

/**
 * @brief TDF框架的时间系统
 * @detail 请根据实际业务场景，仔细斟酌，选用以下时钟之一，获取正确的时间：
 *   【RealTimeClock】真实世界的时钟
 *   【EngineClock】UI系统虚拟时钟，UI操作、以及那些希望跟随UI系统时间轴步速的操作，应采用该时钟获取时间
 */

namespace tdfcore {

/**
 * @brief 时间间隔（单位：ns）
 */
using TDuration = std::chrono::nanoseconds;

/**
 * @brief 默认刷帧间隔
 */
constexpr TDuration kDefaultFrameInterval = TDuration(std::chrono::nanoseconds(1000000000LL / 60LL));

/**
 * @brief 遥远的时长（100年）
 */
constexpr TDuration kDistantDuration = std::chrono::seconds(36525 * 86400LL);

/**
 * @brief 时长的最大值
 */
constexpr TDuration kMaximumDuration = TDuration(INT64_MAX);

/**
 * @brief 时钟：真实世界时间
 */
using RealTimeClock = std::chrono::steady_clock;

/**
 * @brief 基于RealTimeClock的时间类型
 */
using RealTimePoint = std::chrono::time_point<RealTimeClock, TDuration>;

/**
 * @brief 时间计数器，用于累积计数已经流逝的"原子时间单元"总数，即通俗意义上的Tick总数
 * @detail "原子时间单元"是一个抽象的单位，Clock会将其锚定在一个具体的时间间隔上，例如 1Tick = 1ns
 */
class TimeCounter : public Object {
 public:
  ~TimeCounter() override = default;

  /**
   * @brief 获取当前的（原子时间单元）累积计数值
   */
  virtual int64_t GetCount() const = 0;
};

/**
 * @brief 基于真实时间的时间计数器
 */
class RealTimeCounter : public TimeCounter {
 public:
  ~RealTimeCounter() override = default;

  /**
   * @brief 获取当前的（原子时间单元）累积计数值；隐含了1ns为1Tick的设定
   */
  int64_t GetCount() const override { return RealTimeClock::now().time_since_epoch().count(); }

 protected:
  RealTimeCounter() = default;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 时钟：UI系统虚拟时间
 * @detail 本时钟不具有全局唯一性，故需实例化使用，特性如下：
 *   【时间法则】
 *    可从两个维度去影响当前时间的计算：
 *   （1）时间计数器：用于累积计数已经流逝的原子时间单元（Tick）总数，它影响时间的流逝量
 *   （2）时间单元尺度：一个独立时间单元（Tick）所对应的具体时间间隔，它影响时间的流逝速率
 *    当前时间是 量 * 速率 共同叠加的结果
 *
 *   【时间计数器】
 *   （1）同一个时钟，搭载不同的时间计数器，可建立不同性质的时间坐标系
 *   （2）默认采用RealTimeCounter作为时间计数器，它与真实时间完全同步，此为线性真实时间坐标系
 *   （3）可以设置自定义的时间计数器，例如采用ManualTimeCounter，它脱离真实时间，可自由控制时间流逝量，此为非线性自定义时间坐标系
 *
 *   【时间单元尺度】
 *   （1）统一将一个Tick（时间单元）锚定为一个std::chrono::nanoseconds，即：1Tick = 1ns，此为时间流逝的正常速率
 *   （2）通过SetRate(rate)方法，影响原子时间单元的锚定关系，变为：1Tick = rate * 1ns，从而实现时间快放或慢放的效果
 */
class EngineClock : public Object {
 public:
  using TimePoint = std::chrono::time_point<EngineClock, TDuration>;

  ~EngineClock() override = default;

  /**
   * @brief 构造函数
   * @param time_counter 时间计数器，默认使用RealTimeCounter
   */
  explicit EngineClock(const std::shared_ptr<TimeCounter> &time_counter = TDF_MAKE_SHARED(RealTimeCounter));

  /**
   * @brief 获取当前时间戳
   */
  TimePoint Now() const;

  /**
   * @brief 获取当前的TimeCounter
   */
  constexpr const std::shared_ptr<TimeCounter> &GetTimeCounter() const { return time_counter_; }

  /**
   * @brief 设置自定义的TimeCounter；不可为nullptr
   */
  void SetTimeCounter(const std::shared_ptr<TimeCounter> &time_counter);

  /**
   * @brief 获取当前的时间流逝倍率；默认值为1.0
   */
  constexpr float GetRate() const { return rate_; }

  /**
   * @brief 设置时间流逝倍率；自调用时刻起，新的倍率生效，此后时钟将原子时间单元锚定为：rate * 1ns
   * @param rate 倍率；不可为负数
   */
  void SetRate(float rate);

 private:
  std::shared_ptr<TimeCounter> time_counter_;
  int64_t referenced_count_ = 0;
  TimePoint epoch_ = TimePoint();
  float rate_ = 1.0;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 基于EngineClock的时间类型
 */
using TimePoint = EngineClock::TimePoint;

/**
 * @brief 手动控制时间流逝量的时间计数器
 */
class ManualTimeCounter : public TimeCounter {
 public:
  ~ManualTimeCounter() override = default;

  /**
   * @brief 构造函数
   * @param count （原子时间单元）起始计数值
   */
  explicit ManualTimeCounter(int64_t count = 0) : count_(count) { assert(count_ >= 0); }

  /**
   * @brief 获取当前的（原子时间单元）累积计数值
   */
  int64_t GetCount() const override { return count_; }

  /**
   * @brief 增加（原子时间单元）累积计数值
   * @param count 增量，必须为正数
   */
  void Increase(int64_t count) {
    assert(count > 0);
    count_ += count;
  }

 private:
  int64_t count_ = 0;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
