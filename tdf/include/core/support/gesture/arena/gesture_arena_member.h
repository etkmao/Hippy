//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <cstdint>
#include <memory>

#include "core/common/reflection.h"
#include "core/support/gesture/arena/gesture_arena_manager.h"
#include "core/support/gesture/event/pointer_data.h"

namespace tdfcore {
class GestureArenaMember;
/**
 *  GestureArenaLevel 越小优先级越高
 */
using GestureArenaLevel = size_t;

struct GestureArenaAcceptModel {
 public:
  // 对应的 pointer id
  PointerID pointer_id;
  // 回调对应的 member 优先级别，优先级别越低级别越高
  GestureArenaLevel this_member_level;
  // 将要获得竞技场权限的 member
  std::shared_ptr<GestureArenaMember> will_accept_member;
  // 将要获得竞技场权限的 member 优先级别
  GestureArenaLevel will_accept_member_level;
};

/**
 * @brief 手势竞技场中竞技对象的抽象类，参与竞技的对象都会继承自这个类。目前core中所有的手势识别器
 * 都继承了这个类，实际参与竞技的其实是各种手势识别器。
 */
class GestureArenaMember : public Object, public Reflectable,
    public std::enable_shared_from_this<GestureArenaMember> {
  TDF_REFF_CLASS_META(GestureArenaMember)
 public:
  /**
   * @brief 当当前成员在竞技场赢得胜利后会调用这个方法，接收事件处理。
   *
   * @param pointer 事件唯一ID
   */
  virtual void AcceptGesture(PointerID pointer) = 0;

  /**
   * @brief 当当前成员在竞技场失败后会调用这个方法，拒绝事件处理。
   *
   * @param pointer
   */
  virtual void RejectGesture(PointerID pointer) = 0;

  /**
   * @brief 设置当前所属ObjectID
   *
   * @param owner_id
   */
  void SetOwnerId(uint64_t owner_id) { owner_id_ = owner_id; }
  /**
   * @brief 获取
   */
  constexpr uint64_t GetOwnerId() const { return owner_id_; }

  /**
   * @brief 设置手势里面的竞技场
   * @param gesture_arena_manager
   */
  void SetGestureArenaHandle(const std::weak_ptr<GestureArenaHandleInterface> &gesture_arena_handle) {
    gesture_arena_handle_ = gesture_arena_handle;
  }

  /**
   * @brief 当前 member 来决定 will_accept_member 是否可以赢得竞技场胜利
   * @param accept_model 相应数据 model
   * @return true 可以赢得竞技场胜利，false 禁止赢得竞技场胜利
   */
  using MemberShouldAcceptClosure = std::function<bool(const GestureArenaAcceptModel accept_model)>;

  /**
   * @brief 当一个手势推动自己要进入到 Accept
   * 状态时，竞技场会通知到在竞技中的其他手势，其他手势会有权力决定此手势是否可以进入 Accept
   *        此方法用来设置竞技场通知的回调
   * @param memberShouldAcceptClosure 竞技场通知回调
   */
  void SetArenaMemberShouldAccept(const MemberShouldAcceptClosure &member_should_accept_closure) {
    member_should_accept_closure_ = member_should_accept_closure;
  }

  /**
   * @brief 获取竞技场通知某一手势是否可以进入到 Accept 状态的回调
   * @return
   */
  MemberShouldAcceptClosure GetArenaMemberShouldAcceptClosure() { return member_should_accept_closure_; }

  /**
 * @brief 当前 member 来决定是否可以被 will_accept_member 拒绝
 * @param accept_model 相应数据 model
 * @return true 可以被当前Accept手势拒绝（默认），false 禁止被当前Accept手势拒绝
 */
  using MemberCanBeRejectedClosure = std::function<bool(const GestureArenaAcceptModel accept_model)>;

  /**
   * @brief 当一个其他手势进入到 Accept 状态时
   * 竞技场会为当前手势发送一个 Reject，本方法回调来决定当前手势是否可以被 Reject
   * @param memberCanBeRejectClosure 竞技场通知回调
   */
  void SetArenaMemberCanBeRejected(const MemberCanBeRejectedClosure &member_can_be_rejected_closure) {
    member_can_be_rejected_closure_ = member_can_be_rejected_closure;
  }

  /**
   * @brief 获取竞技场通知某一手势是否可以进入到 Accept 状态的回调
   * @return
   */
  MemberCanBeRejectedClosure GetArenaMemberCanBeRejectedClosure() { return member_can_be_rejected_closure_; }

  /**
   * @brief 手势进行事件分发
   * @param event Pointer 事件
   */
  void HandlePointer(const PointerData &data);

  /**
   * @brief 处理 down event 事件
   * @param event
   */
  virtual void HandlePointerDown(const PointerData &data) = 0;

  /**
   * @brief 处理 move event 事件
   * @param event
   */
  virtual void HandlePointerMove(const PointerData &data) = 0;

  /**
   * @brief 处理 up event 事件
   * @param event
   */
  virtual void HandlePointerUp(const PointerData &data) = 0;

  /**
   * @brief 处理 scroll event 事件
   * @param event
   */
  virtual void HandlePointerScroll(const PointerData &data);

  /**
   * @brief 处理 scale event 事件
   * @param event
   */
  virtual void HandlePointerScale(const PointerData &data);

  /**
   * @brief 处理 hover event 事件
   * @param event
   */
  virtual void HandleEventHover(const PointerData &data);

  /**
   * @brief 处理 cancel event 事件
   * @param event
   */
  virtual void HandlePointerCancel(const PointerData &data) = 0;

  ~GestureArenaMember() override = default;

 protected:
  std::shared_ptr<GestureArenaHandleInterface> GetGestureArenaHandle() { return gesture_arena_handle_.lock(); }

 private:
  // 竞技场
  std::weak_ptr<GestureArenaHandleInterface> gesture_arena_handle_;

  // 手势是否可以进入到 Accept 回调
  MemberShouldAcceptClosure member_should_accept_closure_;

  // 手势是否可以被其他手势 Reject 回调
  MemberShouldAcceptClosure member_can_be_rejected_closure_;

  // 当前所属元素ID
  uint64_t owner_id_ = 0;
};

}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::GestureArenaMember, bases<>)
TDF_REFL_END(tdfcore::GestureArenaMember)