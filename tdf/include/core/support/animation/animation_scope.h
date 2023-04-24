//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <stack>
#include <utility>
#include "core/support/animation/animation_controller.h"

namespace tdfcore {
enum class AnimationScopeType {
  kAnimatable,
  kNonAnimatable,
};

// 使用方在动画域内部的行为及相关回调行为
using Action = std::function<void()>;

// 每个Scope持有一个AnimationController，拥有独立的动画能力
using AnimationControllerMap = std::unordered_map<uint64_t, std::shared_ptr<AnimationController>>;

// 属性监听回调，Scope给所有监听者返回当前Scope的动画进度，属性动画自行完成插值
using UpdateListener = std::function<void(double progress)>;

// 记录一个View（由Key标记）对应的所有属性信息（监听器+目标值，即进度为1时的值）
// std::any 属性动画的目标值，如果外界要在动画过程中获取属性值，应该返回这个值，而不是动画的当前值
using AttributeMap = std::unordered_map<std::intptr_t, std::pair<UpdateListener, std::any>>;

// 记录一个Scope（由Key标记）对应的所有View
using ViewMap = std::unordered_map<uint64_t, AttributeMap>;

enum class AnimationScopeState {
  Init,
  Animating,
  Canceled,
  Finished,
};

class AnimationScopeManager;

class AnimationScope final : public Object, public std::enable_shared_from_this<AnimationScope> {
 public:
  AnimationScope(uint64_t id, Action action, const std::weak_ptr<AnimationScopeManager>& scope_manager,
                 TDuration duration = std::chrono::milliseconds(250));

  // 禁止赋值和拷贝域
  AnimationScope(const AnimationScope&) = delete;
  AnimationScope(AnimationScope&&) = delete;
  AnimationScope& operator=(const AnimationScope&) = delete;
  AnimationScope& operator=(AnimationScope&&) = delete;

#define SetCallback(cb) \
  cb = action;          \
  return *this;

  /**
   * @brief 当前动画域启动的回调
   */
  AnimationScope& SetOnStartCallback(const Action& action) { SetCallback(on_start_callback_) }

  /**
   * @brief 当前动画域正常结束的回调
   */
  AnimationScope& SetOnFinishCallback(const Action& action) { SetCallback(on_finish_callback_) }

  /**
   * @brief 当前动画域被取消的回调
   */
  AnimationScope& SetOnCancelCallback(const Action& action) {
    SetCallback(on_cancel_callback_)
  }

#undef SetCallback

  /**
   * 启动动画域内部的属性动画
   */
  void Start();

  /**
   * 结束（完成，等价于动画进度100%）动画域的属性动画
   */
  void Finish();

  /**
   * 取消（未完成，等价于动画进度0%）动画域的属性动画
   */
  void Cancel();

  /**
   * @brief 增加一个属性监听记录，一般无需主动注销，Scope会在条件满足时移除监听，条件:
   *            1. Scope的动画完成
   *            2. 属性被设置的新的值，旧的监听被移除
   *
   * @param view_id 属性所在的view
   * @param attr_id 属性的唯一id
   * @param target 属性的目标值
   * @param callback 属性对于动画进度更新的回调
   */
  void AddAttrInfo(uint64_t view_id, std::intptr_t attr_id, const std::any& target,
                        const UpdateListener& callback);

  /**
   * @brief 移除一个属性监听，即上述条件2
   */
  bool RemoveAttrInfo(uint64_t view_id, std::intptr_t attr_id);

  /**
   * @brief 获取目标属性的动画最终值。如果在当前Scope有登记，说明在动画中，Get到的是动画的目标值
   */
  std::any GetValue(uint64_t view_id, uint64_t attr_id);

  /**
   * 设置动画进度监听，每次Scope执行完action()需要设置一次
   * 后续每次有属性更改时，要重置对应Scope的动画监听
   */
  void SetAnimationListener();

  AnimationScopeState GetState() { return state_; }

 private:
  /**
   * @brief 清理动画信息，销毁所有监听，同时将Scope自身移出AnimationScopeManager
   */
  void ClearScope();

  void OnStart();
  void OnFinish();
  void OnCancel();

  // 当前动画域内要执行的动作
  Action animatable_action_;
  Action on_start_callback_;
  Action on_finish_callback_;
  Action on_cancel_callback_;
  // 当前动画域的状态
  AnimationScopeState state_;
  // 该动画域内部需要进度属性变更的View及其属性
  ViewMap view_map_;
  std::shared_ptr<AnimationController> controller_;
  std::weak_ptr<AnimationScopeManager> scope_manager_;
};

// 记录一个当前存活的所有Scope（由Key标记）。
// 这里使用有序Map,由于IDGenerator::NextUniqueID是自增的，Key直接用其值即可。
// Key的值越大，说明属性信息设置时机越靠后，外部获取信息时逆序遍历查找即可。
using ScopeMap = std::map<uint64_t, std::shared_ptr<AnimationScope>>;

/**
 * @brief 一个支持属性动画的域(Scope)
  数据结构示意
          ┌─────ScopeMap─────────┐
          │         │            │
          ▼         ▼            ▼
     ┌──Scope─┐    Scope       Scope
     │        │      │
     ▼        ▼      ▼
 ViewMap ViewMap ┌─ViewMap────┐
                 │    │       │
                 ▼    ▼       ▼
              Attr  Attr──┐ Attr
                    │     │
                    ▼     ▼
               Target   Callback
 */
class AnimationScopeManager final : public Object, public std::enable_shared_from_this<AnimationScopeManager> {
 public:
  /**
   * @brief 在这个范围内设置的属性，如果支持动画(Tween)，则会按照动画方式更新。默认动画时长 250ms
   */
  AnimationScope& Animatable(Action action, TDuration duration = std::chrono::milliseconds(250));

  /**
   * @brief 在这个范围内设置的属性，无论是否支持动画，都不会按照动画方式更新
   */
  void NonAnimatable(const Action& action);

  /**
   * @brief 是否在属性动画的Scope内
   */
  bool IsAnimatableScope();

  /**
   * @brief 获取当前所在的AnimationScope
   */
  AnimationScope& GetCurrentScope();

  /**
   * @brief 执行属性动画前添加Stack的顶部标记
   */
  void PushStack(uint64_t id);

  /**
   * @brief 属性动画设置结束立即清理Stack（动画本身并未结束）
   */
  void PopStack();

  /**
   * @brief 移除目标Scope
   */
  void RemoveScope(uint64_t id) { scope_map_.erase(id); }

  /**
   * @brief 查询目标Scope是否在，因为Scope的生命周期由AnimationScopeManager统一控制，
   *        外部在使用前检查其有效性。
   */
  bool IsScopeExist(uint64_t id) { return scope_map_.find(id) != scope_map_.end(); }

  /**
   * @brief 移除一个属性监听，重复设置属性时要清理掉之前的注册，因为要遍历所有Scope，所以放在manager中。
   */
  void RemoveAttrInfo(uint64_t view_id, std::intptr_t attr_id);

  /**
   * @brief 获取目标属性的动画最终值。如果在当前Scope有登记，说明在动画中，Get到的是动画的目标值。
   *        因为要遍历所有Scope，所以放在manager中。
   */
  std::any GetValue(uint64_t view_id, uint64_t attr_id);

 private:
  // Scope类型栈，主要用于判断当前是否在一个动画域
  std::stack<AnimationScopeType> scope_type_stack_;

  // Scope Id记录，记录Action *执行时* 栈顶的Scope的id，不能用scope_map_的key，
  // 因为scope_map_的顺序时action的记录顺序，而不是执行顺序
  std::stack<uint64_t> scope_id_stack_;

  // 记录所有的Scope，是一个有序Map
  //  1. 注册监听时，使用 scope_id_stack_ 栈顶的id对应的scope
  //  2. 查询属性时使用 scope_map_ 自顶向下第一个查询到的属性（实际也只能查询到一个）,
  //     查询不到则返回返回空的 std::any ，View判断如果std::any.has_value()为false，则返回RenderObject对应的值
  //  3. scope_id_stack_在出了Scope就出栈，scope_map_则在动画结束后销毁对应Scope的信息，这里是不同的
  ScopeMap scope_map_;
};
}  // namespace tdfcore
