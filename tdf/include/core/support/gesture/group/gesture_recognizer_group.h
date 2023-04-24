//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/gesture/arena/gesture_arena_manager.h"
#include "core/support/gesture/recognizer/gesture_recognizer.h"

namespace tdfcore {
class GestureRecognizerGroupArena;
using HandleEventFunction = std::function<void(std::shared_ptr<GestureRecognizer>)>;

/**
 * @brief 允许添加一序列的手势进入到 GestureRecognizerGroup,
 * 添加到 GestureRecognizerGroup 手势不会互相竞争，允许相互运行
 * 注意，不要添加相同类型的手势进入到 GestureRecognizerGroup 内，否则会有不可预料的场景出现
 */
class GestureRecognizerGroup : public GestureRecognizer {
  TDF_REFF_CLASS_META(GestureRecognizerGroup)
 public:
  GestureRecognizerGroup();

  /**
   * Group 内添加一个手势
   * @param recognizer 目标手势
   */
  void AddGesture(const std::shared_ptr<GestureRecognizer> &recognizer);

  /**
   * 移除 Group 内的一个手势
   * @param recognizer 要移除的手势
   */
  void RemoveGesture(const std::shared_ptr<GestureRecognizer> &recognizer);

 protected:
  bool CanAddPointer(const PointerData &data) const override;

  void AcceptGesture(PointerID pointer) override;
  void RejectGesture(PointerID pointer) override;

  void HandlePointerDown(const PointerData &data) override;
  void HandlePointerMove(const PointerData &data) override;
  void HandlePointerUp(const PointerData &data) override;
  void HandlePointerScroll(const PointerData &data) override;
  void HandlePointerScale(const PointerData &data) override;
  void HandlePointerCancel(const PointerData &data) override;

 private:
  void InitHandleEventQueue(const PointerData &data);
  void HandleEventWithFunction(PointerID pointer, const HandleEventFunction &function);
  void CleanHandleEventQueue(const PointerData &data);

  // 竞技场方法
  void Reject(PointerID pointer, const std::shared_ptr<GestureArenaMember> &member);
  bool Accept(PointerID pointer, const std::shared_ptr<GestureArenaMember> &member);

  std::vector<std::shared_ptr<GestureRecognizer>> recognizers_;
  std::map<PointerID, std::vector<std::shared_ptr<GestureRecognizer>>> handle_event_queue_;
  std::shared_ptr<GestureRecognizerGroupArena> gesture_arena_;

  friend GestureRecognizerGroupArena;
};

/**
 * GestureRecognizerGroup 和 GestureArenaHandleInterface 桥接层
 */
class GestureRecognizerGroupArena : public Object, public GestureArenaHandleInterface {
 public:
  ~GestureRecognizerGroupArena() override = default;

 protected:
  void Reject(PointerID pointer, std::shared_ptr<GestureArenaMember> member) override;
  bool Accept(PointerID pointer, std::shared_ptr<GestureArenaMember> member) override;

 private:
  void SetGestureGroup(const std::weak_ptr<GestureRecognizerGroup> &gesture_group) { gesture_group_ = gesture_group; }
  std::weak_ptr<GestureRecognizerGroup> gesture_group_;

  friend GestureRecognizerGroup;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::GestureRecognizerGroup, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::GestureRecognizerGroup)