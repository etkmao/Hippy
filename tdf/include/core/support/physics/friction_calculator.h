// Copyright 2020 The Tencent Authors. All rights reserved.
#pragma once
#include <cmath>
#include <memory>
#include "core/support/physics/physics.h"
#include "core/support/physics/spring_calculator.h"

namespace tdfcore {

/**
 * @brief 滑动摩擦计算器， 提供模拟有阻尼的情况下的针对offset和velcity的计算
 */
class FrictionCalculator : public PhysicsCalculator {
 public:
  ~FrictionCalculator() override = default;
  static std::shared_ptr<FrictionCalculator> Create(double offset, double velocity);
  FrictionCalculator(double offset, double velocity);
  double GetOffset(TDuration time_delta) override;
  double GetVelocity(TDuration time_delta) override;
  virtual double FinalOffset();

 protected:
  double drag_ = 0.135;
  double draglog_ = std::log(drag_);
};

/**
 * @brief 翻页模式计算器， 提供ScrollView翻页模式下的模拟计算
 */
class ScrollPagingCalculator : public FrictionCalculator {
 public:
  static std::shared_ptr<ScrollPagingCalculator> Create(double current_offset, double velocity, double paging_size);
  ScrollPagingCalculator(double current_offset, double velocity, double paging_size);
  double GetOffset(TDuration time_delta) override;
  double GetVelocity(TDuration time_delta) override;
  double FinalOffset() override;

 protected:
  double calculated_velocity_ = 0.0;
  double paging_offset_ = 0.0;
  SpringDescrition sprint_ = SpringDescrition::CreateWithRadio(0.5, 100, 1.1);
  std::shared_ptr<OverdampedSpringCalculator> over_damped_spring_calculator_;
};
}  // namespace tdfcore
