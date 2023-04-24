//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <cmath>
#include <memory>
#include "core/common/time.h"
#include "core/support/physics/physics.h"

namespace tdfcore {

class CriticalSpringCalculator;
class OverdampedSpringCalculator;
class UnderdampedSpringCalculator;
class SpringCalculator : public PhysicsCalculator {
 public:
  static std::shared_ptr<SpringCalculator> Create(SpringDescrition spring, double offset, double velocity);

 protected:
  SpringCalculator(SpringDescrition spring, double offset, double velocity);
  SpringDescrition spring_;
  double offset_;
  double velocity_;
};

// 临界阻尼
class CriticalSpringCalculator : public SpringCalculator {
 public:
  CriticalSpringCalculator(SpringDescrition spring, double offset, double velocity);
  double GetOffset(TDuration time_delta) override;
  double GetVelocity(TDuration time_delta) override;

 protected:
  double r_;
  double c1_;
  double c2_;
};

// 过阻尼
class OverdampedSpringCalculator : public SpringCalculator {
 public:
  OverdampedSpringCalculator(SpringDescrition spring, double offset, double velocity);
  double GetOffset(TDuration time_delta) override;

  double GetVelocity(TDuration time_delta) override;

 protected:
  double r1_;
  double r2_;
  double c1_;
  double c2_;
};

// 欠阻尼
class UnderdampedSpringCalculator : public SpringCalculator {
 public:
  UnderdampedSpringCalculator(SpringDescrition spring, double offset, double velocity);
  double GetOffset(TDuration time_delta) override;

  double GetVelocity(TDuration time_delta) override;

 protected:
  double w_;
  double r_;
  double c1_;
  double c2_;
};

}  // namespace tdfcore
