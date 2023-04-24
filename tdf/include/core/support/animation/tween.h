//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <utility>
#include "core/common/color.h"
#include "core/common/m33.h"
#include "core/common/object.h"
#include "core/common/rect.h"
#include "core/support/animation/curves.h"
#include "core/support/animation/m44_mixer.h"

namespace tdfcore {

static double GetValueByPercent(double begin, double end, double percent) { return begin + (end - begin) * percent; }

template <typename T>
class Tween : public Object {
 public:
  ~Tween() override = default;
  Tween(T begin, T end, std::shared_ptr<Curve> curve = TDF_MAKE_SHARED(LinearCurve))
  : begin_(begin), end_(end), curve_(std::move(curve)) {}

  /**
   * @brief 补间插值计算
   * @param percent 0~1
   */
  T Lerp(double percent) {
    assert(percent >= 0.0 && percent <= 1.0);
    double current_percent = std::clamp(curve_->Transform(percent), 0.0, 1.0);
    return begin_ + (end_ - begin_) * current_percent;
  }

  constexpr const T &GetBegin() const { return begin_; }
  constexpr const T &GetEnd() const { return end_; }

 protected:
  T begin_;
  T end_;
  std::shared_ptr<Curve> curve_;

  FRIEND_OF_TDF_ALLOC;
};

template <>
class Tween<TM33> : public Object {
 public:
  ~Tween() override = default;
  Tween(const TM33 &begin, const TM33 &end, std::shared_ptr<Curve> curve = TDF_MAKE_SHARED(LinearCurve))
  : begin_(begin), end_(end), curve_(std::move(curve)) {
    begin_translation_ = {0.0, 0.0, 0.0};
    end_translation_ = {0.0, 0.0, 0.0};

    begin_scale_ = {0.0, 0.0, 0.0};
    end_scale_ = {0.0, 0.0, 0.0};

    begin_rotation_ = {0.0, 0.0, 0.0};
    end_rotation_ = {0.0, 0.0, 0.0};

    TV4 begin_rotation = {0.0, 0.0, 0.0, 1.0};
    TV4 end_rotation = {0.0, 0.0, 0.0, 1.0};

    TM44Mixer::Decompose(TM44(begin_), begin_translation_, begin_scale_, begin_rotation);
    TM44Mixer::Decompose(TM44(end_), end_translation_, end_scale_, end_rotation);
  }
  /**
   * @brief 补间插值计算
   * @param percent 0~1
   */
  TM33 Lerp(double percent) {
    assert(percent >= 0.0 && percent <= 1.0);
    double current_percent = std::clamp(curve_->Transform(percent), 0.0, 1.0);

    auto lerp_translation = begin_translation_ + (end_translation_ - begin_translation_) * current_percent;
    auto lerp_scale = begin_scale_ + (end_scale_ - begin_scale_) * current_percent;
    auto lerp_rotation = begin_rotation_ + (end_rotation_ - begin_rotation_) * current_percent;
    return TM44Mixer::Compose(lerp_translation, lerp_scale, lerp_rotation).asM33();
  }

  /**
   * @brief 设置初始旋转弧度
   * @param begin_rotation 初始旋转弧度
   *     绕 x 轴旋转 begin_rotation.x 弧度; 绕 y 轴旋转 begin_rotation.y 弧度; 绕 z 轴旋转
   * begin_rotation.z 弧度; 默认 {0.0, 0.0, 0.0}.
   */
  constexpr void SetBeginRotation(const TV3 &begin_rotation) { begin_rotation_ = begin_rotation; }
  /**
   * @brief 设置结束旋转弧度
   * @param end_rotation 结束旋转弧度
   *     绕 x 轴旋转 end_rotation.x 弧度; 绕 y 轴旋转 end_rotation.y 弧度; 绕 z 轴旋转
   * end_rotation.z 弧度; 默认 {0.0, 0.0, 0.0}.
   */
  constexpr void SetEndRotation(const TV3 &end_rotation) { end_rotation_ = end_rotation; }
  constexpr void SetBeginTranslation(const TV3 &begin_translation) { begin_translation_ = begin_translation; }
  constexpr void SetEndTranslation(const TV3 &end_translation) { end_translation_ = end_translation; }
  constexpr void SetBeginScale(const TV3 &begin_scale) { begin_scale_ = begin_scale; }
  constexpr void SetEndScale(const TV3 &end_scale) { end_scale_ = end_scale; }

  constexpr const TM33 &GetBegin() const { return begin_; }
  constexpr const TM33 &GetEnd() const { return end_; }

 protected:
  TM33 begin_;
  TM33 end_;
  std::shared_ptr<Curve> curve_;

 private:
  TV3 begin_rotation_{};
  TV3 end_rotation_{};
  TV3 begin_translation_{};
  TV3 end_translation_{};
  TV3 begin_scale_{};
  TV3 end_scale_{};
};

template <>
class Tween<Color> : public Object {
 public:
  ~Tween() override = default;
  Tween(Color begin, Color end, std::shared_ptr<Curve> curve = TDF_MAKE_SHARED(LinearCurve))
  : begin_(begin), end_(end), curve_(std::move(curve)) {}

  /**
   * @brief 补间插值计算
   * @param percent 0~1
   */
  Color Lerp(double percent) {
    assert(percent >= 0.0 && percent <= 1.0);
    double current_percent = std::clamp(curve_->Transform(percent), 0.0, 1.0);
    double a = GetValueByPercent(begin_.GetA(), end_.GetA(), current_percent);
    double r = GetValueByPercent(begin_.GetR(), end_.GetR(), current_percent);
    double g = GetValueByPercent(begin_.GetG(), end_.GetG(), current_percent);
    double b = GetValueByPercent(begin_.GetB(), end_.GetB(), current_percent);
    return Color::ARGB(a, r, g, b);
  }

  constexpr const Color &GetBegin() const { return begin_; }
  constexpr const Color &GetEnd() const { return end_; }

 protected:
  Color begin_;
  Color end_;
  std::shared_ptr<Curve> curve_;
};

template <>
class Tween<TM44> : public Object {
 public:
  ~Tween() override = default;
  Tween(const TM44 &begin, const TM44 &end, std::shared_ptr<Curve> curve = TDF_MAKE_SHARED(LinearCurve))
  : begin_(begin), end_(end), curve_(std::move(curve)) {
    begin_translation_ = {0.0, 0.0, 0.0};
    end_translation_ = {0.0, 0.0, 0.0};

    begin_scale_ = {0.0, 0.0, 0.0};
    end_scale_ = {0.0, 0.0, 0.0};

    begin_rotation_ = {0.0, 0.0, 0.0};
    end_rotation_ = {0.0, 0.0, 0.0};

    TV4 begin_rotation = {0.0, 0.0, 0.0, 1.0};
    TV4 end_rotation = {0.0, 0.0, 0.0, 1.0};

    TM44Mixer::Decompose(begin_, begin_translation_, begin_scale_, begin_rotation);
    TM44Mixer::Decompose(end_, end_translation_, end_scale_, end_rotation);
  }
  /**
   * @brief 补间插值计算
   * @param percent 0~1
   */
  TM44 Lerp(double percent) {
    assert(percent >= 0.0 && percent <= 1.0);
    double current_percent = std::clamp(curve_->Transform(percent), 0.0, 1.0);

    auto lerp_translation = begin_translation_ + (end_translation_ - begin_translation_) * current_percent;
    auto lerp_scale = begin_scale_ + (end_scale_ - begin_scale_) * current_percent;
    auto lerp_rotation = begin_rotation_ + (end_rotation_ - begin_rotation_) * current_percent;
    return TM44Mixer::Compose(lerp_translation, lerp_scale, lerp_rotation);
  }

  /**
   * @brief 设置初始旋转弧度
   * @param begin_rotation 初始旋转弧度
   *     绕 x 轴旋转 begin_rotation.x 弧度; 绕 y 轴旋转 begin_rotation.y 弧度; 绕 z 轴旋转
   * begin_rotation.z 弧度; 默认 {0.0, 0.0, 0.0}.
   */
  constexpr void SetBeginRotation(const TV3 &begin_rotation) { begin_rotation_ = begin_rotation; }
  /**
   * @brief 设置结束旋转弧度
   * @param end_rotation 结束旋转弧度
   *     绕 x 轴旋转 end_rotation.x 弧度; 绕 y 轴旋转 end_rotation.y 弧度; 绕 z 轴旋转
   * end_rotation.z 弧度; 默认 {0.0, 0.0, 0.0}.
   */
  constexpr void SetEndRotation(const TV3 &end_rotation) { end_rotation_ = end_rotation; }
  constexpr void SetBeginTranslation(const TV3 &begin_translation) { begin_translation_ = begin_translation; }
  constexpr void SetEndTranslation(const TV3 &end_translation) { end_translation_ = end_translation; }
  constexpr void SetBeginScale(const TV3 &begin_scale) { begin_scale_ = begin_scale; }
  constexpr void SetEndScale(const TV3 &end_scale) { end_scale_ = end_scale; }

  constexpr const TM44 &GetBegin() const { return begin_; }
  constexpr const TM44 &GetEnd() const { return end_; }

 protected:
  TM44 begin_;
  TM44 end_;
  std::shared_ptr<Curve> curve_;

 private:
  TV3 begin_rotation_{};
  TV3 end_rotation_{};
  TV3 begin_translation_{};
  TV3 end_translation_{};
  TV3 begin_scale_{};
  TV3 end_scale_{};
};

template <>
class Tween<TRect> : public Object {
 public:
  Tween(const TRect &begin, const TRect &end, std::shared_ptr<Curve> curve = TDF_MAKE_SHARED(LinearCurve))
  : begin_(begin), end_(end), curve_(std::move(curve)) {}
  ~Tween() override = default;

  /**
   * @brief 补间插值计算
   * @param percent 0~1
   */
  TRect Lerp(double percent) {
    assert(percent >= 0.0 && percent <= 1.0);
    double current_percent = std::clamp(curve_->Transform(percent), 0.0, 1.0);
    return TRect::MakeLTRB(GetValueByPercent(begin_.left, end_.left, current_percent),
                           GetValueByPercent(begin_.top, end_.top, current_percent),
                           GetValueByPercent(begin_.right, end_.right, current_percent),
                           GetValueByPercent(begin_.bottom, end_.bottom, current_percent));
  }

  constexpr const TRect &GetBegin() const { return begin_; }
  constexpr const TRect &GetEnd() const { return end_; }

 protected:
  TRect begin_;
  TRect end_;
  std::shared_ptr<Curve> curve_;
};

}  // namespace tdfcore
