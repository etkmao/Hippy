//
// Copyright (c) 2020 Tencent. All rights reserved.

//
#pragma once
#include <cmath>
#include <vector>
#include "core/common/macro.h"

namespace tdfcore {
class LeastSquaresSolver {
 public:
  LeastSquaresSolver() : ssr_(0), sse_(0), rmse_(0) { factor_.resize(2, 0); }
  ~LeastSquaresSolver() = default;

  /**
   * @brief 直线拟合-一元回归,拟合的结果使用getFactor获取，使用getSlope获取斜率，getIntercept获取截距
   * @param x 观察值的x
   * @param y 观察值的y
   * @param isSaveFitYs 拟合后的数据是否保存，默认否
   */
  bool LinearFit(const std::vector<double>& x, const std::vector<double>& y, bool is_save_fit_ys = false);

  /**
   * @brief 根据阶次获取拟合方程的系数，如getFactor(2),就是获取y=a0+a1*x+a2*x^2+……+apoly_n*x^poly_n中a2的值
   * @return 拟合方程的系数
   */
  double GetFactor(size_t i) { return factor_.at(i); }

 private:
  std::vector<double> factor_;
  // 拟合后的方程系数
  double ssr_;
  // 回归平方和
  double sse_;
  // 剩余平方和
  double rmse_;
  // RMSE均方根误差
  std::vector<double> fited_ys_;

  size_t GetSeriesLength(const std::vector<double>& x, const std::vector<double>& y) {
    return (x.size() > y.size() ? y.size() : x.size());
  }

  static double Mean(const std::vector<double> &v);

  double GetY(const double &x) const;

  void CalcError(const std::vector<double> &x, const std::vector<double> &y,
                 double& r_ssr, double& r_sse, double& r_rmse, bool is_save_fit_ys = true);
};
}  // namespace tdfcore
