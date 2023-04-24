//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include "core/common/memory.h"
#include "core/common/object.h"

namespace tdfcore {

class Curve : public Object {
 public:
  ~Curve() override = default;
  virtual double Transform(double t) = 0;

 protected:
  Curve() = default;

  FRIEND_OF_TDF_ALLOC;
};

class LinearCurve : public Curve {
 public:
  ~LinearCurve() override = default;
  double Transform(double t) override { return t; }

 protected:
  LinearCurve() = default;

  FRIEND_OF_TDF_ALLOC;
};

class SawTooth : public Curve {
 public:
  ~SawTooth() override = default;
  double Transform(double t) override;

 protected:
  explicit SawTooth(int count);
  SawTooth() = default;

 private:
  int count_ = 0;

  FRIEND_OF_TDF_ALLOC;
};

class Cubic : public Curve {
 public:
  ~Cubic() override = default;
  static std::shared_ptr<Cubic> MakeEase();
  static std::shared_ptr<Cubic> MakeEaseIn();
  static std::shared_ptr<Cubic> MakeEaseOut();
  static std::shared_ptr<Cubic> MakeEaseInOut();
  static std::shared_ptr<Cubic> MakeFastOutSlowIn();

  double Transform(double t) override;

 protected:
  Cubic(double a, double b, double c, double d);
  static double EvaluateCubic(double a, double b, double m);

 private:
  double a_;
  double b_;
  double c_;
  double d_;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
