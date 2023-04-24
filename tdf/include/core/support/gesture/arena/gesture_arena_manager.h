//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <iostream>
#include <list>
#include <unordered_map>
#include <memory>
#include "core/support/gesture/event/pointer_data.h"

namespace tdfcore {
class GestureArenaMember;
class GestureArena;

/**
 * @brief 手势竞技场功能协议
 */
class GestureArenaHandleInterface {
 public:
  /**
   * 禁止手势获得竞技场胜利
   * @param pointer 对应的竞技场 ID
   * @param member 被禁止的手势
   */
  virtual void Reject(PointerID pointer, std::shared_ptr<GestureArenaMember> member) = 0;

  /**
   * 尝试让手势获取竞技场胜利
   * @param pointer 对应的竞技场 ID
   * @param member 相应手势
   * @return true 获得竞技场胜利，false 未获得竞技场胜利
   */
  virtual bool Accept(PointerID pointer, std::shared_ptr<GestureArenaMember> member) = 0;
};

/**
 * @brief 手势竞技场的管理类，内部包含各种竞技场的管理动作。
 */
class GestureArenaManager : public Object, public GestureArenaHandleInterface {
 public:
  ~GestureArenaManager() override = default;

  void Reject(PointerID pointer, std::shared_ptr<GestureArenaMember> member) override;
  bool Accept(PointerID pointer, std::shared_ptr<GestureArenaMember> member) override;

  /**
   * @brief 根据相应的事件 ID 查找到相应的竞技场往其添加手势
   * @param pointer 事件 ID
   * @param member 手势
   */
  void Add(PointerID pointer, const std::shared_ptr<GestureArenaMember>& member);

  /**
   * 竞技场分发事件给竞技场的手势
   * @param data 分发的事件
   */
  void HandlePointer(const PointerData &data);

 private:
  std::unordered_map<PointerID, std::shared_ptr<GestureArena>> arenas_;

  bool MemberCanAccept(PointerID pointer, std::shared_ptr<GestureArenaMember> member);

  bool MemberCanBeRejected(PointerID pointer, std::shared_ptr<GestureArenaMember> current_member,
                           std::shared_ptr<GestureArenaMember> will_accept_member);
};

/**
 * @brief 手势竞技场，内部包含了竞技场的状态信息，以及竞技成员信息。
 */
class GestureArena : public Object {
 public:
  ~GestureArena() override = default;

 protected:
  GestureArena() = default;

 private:
  const std::vector<std::weak_ptr<GestureArenaMember>> &GetMembers() const { return members_; }

  void AddMember(const std::shared_ptr<GestureArenaMember>& member) { members_.push_back(member); }
  void RemoveMember(const std::shared_ptr<GestureArenaMember>& member);

  std::vector<std::weak_ptr<GestureArenaMember>> members_;

  friend GestureArenaManager;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
