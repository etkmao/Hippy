//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <map>
#include <memory>
#include <set>
#include <utility>
#include "core/support/gesture/event/pointer_data.h"
#include "core/support/gesture/recognizer/gesture_recognizer.h"

namespace tdfcore {
/**
 * @brief 监听同一顺序内的所有手势事件
 */
class OneSequenceGestureRecognizer : public GestureRecognizer {
  TDF_REFF_CLASS_META(OneSequenceGestureRecognizer)
 protected:
  void HandlePointerDown(const PointerData &data) override;
  void HandlePointerMove(const PointerData &data) override;
  void HandlePointerUp(const PointerData &data) override;
  void HandlePointerCancel(const PointerData &data) override;

  virtual void Reject();
  virtual bool Accept();

  /**
   * @brief 开始处理事件处理，将手势识别的器的事件处理逻辑注册到事件路由，并将手势识别器添加到手势竞技场中去。
   * @param pointer 事件的唯一ID
   * @param transform
   */
  void StartTrackingPointer(PointerID pointer);

  /**
   * @brief 根据事件ID删除之前注册到事件路由中的事件处理逻辑。
   *
   * @param pointer 事件的唯一ID
   */
  void StopTrackingPointer(PointerID pointer);

  /**
   * @brief pointer 是否仍然在跟踪范围内
   * @param pointer
   * @return
   */
  bool IsTrackingPointer(PointerID pointer) const;

  /**
   * @brief 当手势识别器追踪的pointer的数量从1变成0的时候调用这个方法，表示同一顺序下的这次监听已经结束。
   *
   * @param pointer 上次追踪事件的事件ID
   */
  virtual void DidStopTrackingLastPointer(PointerID pointer) = 0;

  ~OneSequenceGestureRecognizer() override = default;

 private:
  std::set<PointerID> tracked_pointers_ = std::set<PointerID>();
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::OneSequenceGestureRecognizer, bases<tdfcore::GestureRecognizer>)
TDF_REFL_END(tdfcore::OneSequenceGestureRecognizer)