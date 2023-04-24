//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/object.h"
#include "core/common/time.h"
#include "core/common/point.h"
#include "core/common/rect.h"
#include "core/common/size.h"
#include "core/support/physics/physics.h"
#include "core/support/physics/friction_calculator.h"
#include "core/support/physics/spring_calculator.h"

namespace tdfcore {

/**
 * @brief 设置ScrollView的ContentRect时，对应的滑动策略
 */
class ScrollableRectStrategy : public Object {
 public:
  /**
   * @brief 通过传入的scroll_view 和content_rect来计算该scroll_view可以滑动的距离，子类需要重写该方法
   * @param scroll_view 需要计算滑动距离的scroll_view
   * @param content_rect 使用者传入的content_rect
   * @return 计算后的content_rect
   */
  virtual TRect CalculateScrollableRect(TRect scrollable_rect) = 0;
};

/**
 * @brief 默认策略，ContentRect小于ScrollView的Frame部分不可以滑动， 大于部分可以滑动
 */
class DefaultScrollableRectStrategy : public ScrollableRectStrategy {
 public:
  ~DefaultScrollableRectStrategy() override = default;
  TRect CalculateScrollableRect(TRect scrollable_rect) override;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 吸顶策略，ContentRect小于ScrollView的Frame时，ContentRect的top与ScrollView的top对齐，同时不可滑动
 */
class TopScrollableRectStrategy : public ScrollableRectStrategy {
 public:
  ~TopScrollableRectStrategy() override = default;
  TRect CalculateScrollableRect(TRect scrollable_rect) override;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 居中策略，ContentRect小于ScrollView的Frame时，ContentRect居中，同时不可滑动
 */
class CenterScrollableRectStrategy : public ScrollableRectStrategy {
 public:
  ~CenterScrollableRectStrategy() override = default;
  TRect CalculateScrollableRect(TRect scrollable_rect) override;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 吸底策略，ContentRect小于ScrollView的Frame时，ContentRect的bottom与ScrollView的bottom对齐，同时不可滑动
 */
class BottomScrollableRectStrategy : public ScrollableRectStrategy {
 public:
  ~BottomScrollableRectStrategy() override = default;
  TRect CalculateScrollableRect(TRect scrollable_rect) override;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief ScrollView使用的物理模块， ScrollView的物理效果由该类来实现
 */
class ScrollPhysics : public Object {
 public:
  /**
   * @brief 计算duration时间后的位移和速度, 外部需要通过该方法计算后再取位移和速度
   * @param duration 时间
   */
  virtual void CalculateOffsetAndVelocity(TDuration duration) = 0;

  /**
   * @brief 请求是否需要有物理效果
   * @return 是否需要有物理效果
   */
  virtual bool PhysicsShouldStart() = 0;

  /**
   * @brief 设置位移，物理模块会根据位移、速度等变量来判断是否需要物理效果
   * @param offset 位移
   */
  virtual void SetOffset(TPoint offset) = 0;

  /**
   * @brief 获取一段时间后的位移
   * @return 一段时间后的位移
   */
  virtual TPoint GetOffset() const = 0;

  /**
   * @brief 设置速度，物理模块会根据位移、速度等变量来判断是否需要物理效果
   * @param offset 位移
   */
  virtual void SetVelocity(TPoint velocity) = 0;

  /**
   * @brief 获取一段时间后的速度
   * @return 一段时间后的速度
   */
  virtual TPoint GetVelocity() const = 0;

  /**
   * @brief 一段时间后物理效果是否完成
   * @return 是否完成
   */
  virtual bool IsDone() = 0;

  /**
   * @brief 设置该模块使用者view的大小
   * @param size 使用者view的大小
   */
  virtual void SetFrameSize(TSize size) = 0;

  /**
   * @brief 获取该模块使用者view的大小
   * @return 使用者view的大小
   */
  virtual TSize GetFrameSize() const = 0;

  /**
   * @brief 设置可以四周可以滑动的矩形，物理模块在该矩形范围中滑动。
   * 物理模块会根据位移、速度、Rect等变量来判断是否需要物理效果
   * @param rect 矩形范围
   */
  virtual void SetScrollableRect(TRect rect) = 0;

  /**
   * @brief 获取四周滑动的矩形范围
   * @return 矩形范围
   */
  virtual TRect GetScrollableRect() const = 0;

  /**
   * @brief 将offset 应用物理效果后返回，如OverScroll效果等
   * @param old_offset 上一次的offset
   * @param offset 本次的Offset
   * @return 应用物理效果后的offset
   */
  virtual TPoint TransformOffset(TPoint old_offset, TPoint offset) = 0;

  /**
   * @brief 获取当前是否为OverScroll状态
   * @return
   */
  virtual bool IsOverScrolled() const = 0;

  /**
   * @brief 设置横向是否允许Overscroll
   * @param enabled 是否允许
   */
  virtual void SetHorizontalOverScrollEnabled(bool enabled) = 0;

  /**
   * @brief 获取横向是否允许Overscroll
   * @return 是否允许
   */
  virtual bool GetHorizontalOverScrollEnabled() const = 0;

  /**
   * @brief 设置纵向是否允许Overscroll
   * @param enabled 是否允许
   */
  virtual void SetVerticalOverScrollEnabled(bool enabled) = 0;

  /**
   * @brief 获取纵向是否允许Overscroll
   * @return 是否允许
   */
  virtual bool GetVerticalOverScrollEnabled() const = 0;

 protected:
  ScrollPhysics() = default;

  FRIEND_OF_TDF_ALLOC;
};

class DefaultScrollPhysics : public ScrollPhysics {
 public:
  ~DefaultScrollPhysics() override = default;

  bool PhysicsShouldStart() override;

  TPoint TransformOffset(TPoint old_offset, TPoint offset) override;

  void CalculateOffsetAndVelocity(TDuration duration) override;

  void SetOffset(TPoint offset) override { offset_ = offset; }

  TPoint GetOffset() const override { return offset_; }

  /**
   * @brief 设置速度，物理模块会根据位移、速度等变量来判断是否需要物理效果
   * @param offset 位移
   */
  void SetVelocity(TPoint velocity) override { velocity_ = velocity; }

  TPoint GetVelocity() const override { return velocity_; }

  void SetScrollableRect(TRect rect) override;

  TRect GetScrollableRect() const override { return origin_rect_; }

  bool IsDone() override;

  void SetHorizontalOverScrollEnabled(bool enabled) override;

  bool GetHorizontalOverScrollEnabled() const override { return horizontal_over_scroll_enabled_; }

  void SetVerticalOverScrollEnabled(bool enabled) override;

  bool GetVerticalOverScrollEnabled() const override { return vertical_over_scroll_enabled_; }

  void SetFrameSize(TSize size) override { frame_size_ = size; }

  TSize GetFrameSize() const override { return frame_size_; }

  bool IsOverScrolled() const override { return is_over_scrolled_; }

  /**
   * @brief 设置是否为分页模式
   * @param paging_enabled_ 是否分页
   */
  void SetPagingEnabled(bool paging_enabled) { paging_enabled_ = paging_enabled; }

  /**
   * @brief 获取是否为分页模式
   * @return 是否分页
   */
  bool GetPagingEnabled() const { return paging_enabled_; }

  /**
   * @brief 计算OverScroll状态下 减小后的offset，随着OverOffset的增加，减小后的offset的增加速度会越来越慢
   * @param over_offset OverScroll状态下的位移
   * @param delta OverScroll状态下的增量
   * @param frame_size  使用者View的大小
   * @return 经过计算 减小后的offset
   */
  double CalculateNarrowOverScroll(float over_offset, double delta, float frame_size) const;

  /**
   * @brief 当Scrollable可以大小小于0时设置Scrollable的显示策略：默认策略、居顶策略、居中策略、居底策略
   * 通过该方法可以替换不同策略，如果需要自定义策略，需要继承ScrollableRectStrategy并重写相关方法
   * @param scrollable_strategy 可滑动区域策略
   */
  void SetScrollableRectStrategy(const std::shared_ptr<ScrollableRectStrategy> &scrollable_strategy) {
    scrollable_strategy_ = scrollable_strategy;
  }

  /**
   * @brief 获取可滑动区域策略
   * @return 可滑动区域策略
   */
  constexpr const std::shared_ptr<ScrollableRectStrategy> &GetScrollableRectStrategy() const {
    return scrollable_strategy_;
  }

 protected:
  enum class CalculatorType { kNone, kFriction, kOverScroll, kPaging };

  explicit DefaultScrollPhysics(const SpringDescrition &horizontal_spring = SpringDescrition(),
                                const SpringDescrition &vertical_spring = SpringDescrition());
  bool NeedsHorizontalPhysics() const;
  bool NeedsVerticalPhysics() const;
  void SetHorizontalCalculator();
  void SetVerticalCalculator();
  TRect origin_rect_;
  TRect scrollable_rect_;
  std::shared_ptr<ScrollableRectStrategy> scrollable_strategy_;
  TSize frame_size_;
  TPoint offset_;
  TPoint velocity_;
  bool horizontal_over_scroll_enabled_ = false;
  bool vertical_over_scroll_enabled_ = false;
  SpringDescrition horizontal_spring_;
  SpringDescrition vertical_spring_;
  std::shared_ptr<PhysicsCalculator> horizontal_calculator_;
  std::shared_ptr<PhysicsCalculator> vertical_calculator_;
  bool spring_status_ = false;
  bool paging_enabled_ = false;
  CalculatorType horizontal_calculator_type_ = CalculatorType::kNone;
  CalculatorType vertical_calculator_type_ = CalculatorType::kNone;
  TDuration horizontal_last_duration_ = TDuration(0);
  TDuration vertical_last_duration_ = TDuration(0);
  TDuration horizontal_start_duration_ = TDuration(0);
  TDuration vertical_start_duration_ = TDuration(0);
  double horizontal_calculator_translate_;
  double vertical_calculator_translate_;
  bool is_over_scrolled_ = false;
  double overscroll_eff = 2.0 / 3.0;
  FRIEND_OF_TDF_ALLOC;
};

class ClampScrollPhysics {};

class BouncingScrollPhysics {};

}  // namespace tdfcore
