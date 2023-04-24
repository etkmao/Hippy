//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include <cmath>
#include <memory>
#include "core/common/object.h"
#include "core/common/time.h"

namespace tdfcore {

namespace number {
const double e = std::exp(1);
}  // namespace number

struct SpringDescrition {
  // F = ma
  double mass = 0.5;
  // F = stiffness * delta;
  double stiffness = 100.;
  // F = -v * damping
  double damping = 15.556349;

  static SpringDescrition CreateWithRadio(double mass = 0.5, double stiffness = 100., double ratio = 1.1);
};

class PhysicsCalculator : public Object {
 public:
  ~PhysicsCalculator() override = default;
  virtual double GetOffset(TDuration time_delta) = 0;
  virtual double GetVelocity(TDuration time_delta) = 0;

 protected:
  PhysicsCalculator(double offset, double velocity);
  double offset_;
  double velocity_;
};

}  // namespace tdfcore
