//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <vector>
#include "core/common/color.h"
#include "core/common/m33.h"
#include "core/common/object.h"
#include "core/common/rect.h"
#include "core/engine/graphics/shader.h"

namespace tdfcore {
/**
 * @brief 渐变抽象类
 */
class Gradient : public Object {
 public:
  /**
   * @brief 构造函数
   * @param colors 渐变颜色数组
   * @param stops 起止点数组，需要和颜色数组大小一致
   * @param mode 平铺模式，作用于起止点之外的边缘区域
   * @param rotate_degree 渐变的旋转角度
   */
  Gradient(std::vector<Color> colors, std::vector<float> stops, TileMode mode, float rotate_degree)
      : colors_(std::move(colors)), stops_(std::move(stops)), tile_mode_(mode), rotate_degree_(rotate_degree) {}

  /**
   * @brief 生成一个SkShader
   */
  virtual std::shared_ptr<Shader> CreateShader(const TRect &bounds) = 0;

 protected:
  std::vector<Color> colors_;
  std::vector<float> stops_;
  TileMode tile_mode_;
  float rotate_degree_;
};

/**
 * @brief 线性渐变
 */
class LinearGradient : public Gradient {
 public:
  ~LinearGradient() override = default;
  /**
   * @brief 构造函数
   * @param begin 渐变开始位置，左上角为(0,0) 右下角为(1,1)
   * @param end 渐变结束位置，左上角为(0,0) 右下角为(1,1)
   * @param colors 渐变颜色数组
   * @param stops 渐变起止点数组，需要和colors数组大小一致
   * @param mode 平铺模式，作用于起止点之外的边缘区域
   * @param rotate_degree 渐变的旋转角度
   */
  LinearGradient(TPoint begin, TPoint end, const std::vector<Color> &colors, const std::vector<float> &stops,
                 TileMode tile_mode = TileMode::kClamp, float rotate_degree = 0);

 protected:
  std::shared_ptr<Shader> CreateShader(const TRect &bounds) override;

 private:
  TPoint begin_;
  TPoint end_;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 径向渐变
 */
class RadialGradient : public Gradient {
 public:
  ~RadialGradient() override = default;
  /**
   * @brief 构造函数
   * @param center 渐变中心点位置，中心点为(0.5, 0.5)
   * @param radius 半径大小
   * @param colors 渐变颜色数组
   * @param stops 渐变起止点数组，需要和colors数组大小一致
   * @param mode 平铺模式，作用于起止点之外的边缘区域
   * @param rotate_degree 渐变的旋转角度
   */
  RadialGradient(TPoint center, float radius, const std::vector<Color> &colors, const std::vector<float> &stops,
                 TileMode tile_mode = TileMode::kClamp, float rotate_degree = 0);

 protected:
  std::shared_ptr<Shader> CreateShader(const TRect &bounds) override;

 private:
  TPoint center_;
  float radius_;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 扫描渐变
 */
class SweepGradient : public Gradient {
 public:
  /**
   * @brief 构造函数
   * @param center 渐变中心点位置，中心点为(0.5, 0.5)
   * @param colors 渐变颜色数组
   * @param stops 渐变起止点数组，需要和colors数组大小一致
   * @param start_angle 起始角度
   * @param end_angle 结束角度
   * @param mode 平铺模式，作用于起止点之外的边缘区域
   * @param rotate_degree 渐变的旋转角度
   */
  SweepGradient(TPoint center, const std::vector<Color> &colors, const std::vector<float> &stops, float start_angle = 0,
                float end_angle = 360, TileMode tile_mode = TileMode::kClamp, float rotate_degree = 0);

 protected:
  std::shared_ptr<Shader> CreateShader(const TRect &bounds) override;

 private:
  TPoint center_;
  float start_angle_;
  float end_angle_;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 锥形渐变，由两个圆渐变效果组成
 */
class TwoPointConicalGradient : public Gradient {
 public:
  /**
   * @brief 构造函数
   * @param start_center 开始圈中心点位置，中心点为(0.5, 0.5)
   * @param start_radius 开始圈半径
   * @param end_center 结束圈中心点位置，中心点为(0.5, 0.5)
   * @param end_radius 结束圈半径
   * @param colors 渐变颜色数组
   * @param stops 渐变起止点数组，需要和colors数组大小一致
   * @param mode 平铺模式，作用于起止点之外的边缘区域
   * @param rotate_degree 渐变的旋转角度
   */
  TwoPointConicalGradient(TPoint start_center, float start_radius, TPoint end_center, float end_radius,
                          const std::vector<Color> &colors, const std::vector<float> &stops,
                          TileMode tile_mode = TileMode::kClamp, float rotate_degree = 0);

 protected:
  std::shared_ptr<Shader> CreateShader(const TRect &bounds) override;

 private:
  TPoint start_center_;
  float start_radius_;
  TPoint end_center_;
  float end_radius_;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
