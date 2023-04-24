//
// Copyright (c) 2021 Tencent Corporation. All rights reserved.
//

#pragma once

#include <condition_variable>
#include <mutex>
#include "core/common/object.h"
#include "core/common/time.h"

namespace tdfcore {

/**
 * @brief 可进行等待与异步唤醒的对象
 * @detail 采用条件变量实现；
 *         在线程A中调用Wait方法将进行阻塞式等待，在线程B中调用Notify方法解除线程A的阻塞，同时标记notified_状态为true；
 *         Wait方法仅在notified_状态为false时，才会进行阻塞式等待，可调用Reset方法重置notified_状态为false
 */
class WaitableObject : public Object {
 public:
  ~WaitableObject() override = default;

  /**
   * @brief 构造函数
   * @param auto_reset 是否在每次被Notify()后，自动重置notified_标志
   */
  explicit WaitableObject(bool auto_reset = true);

  WaitableObject(const WaitableObject&) = delete;
  WaitableObject& operator=(const WaitableObject&) = delete;

  /**
   * @brief 当notified_状态为false时，进行阻塞式等待，直到出现Notify调用；否则不阻塞，直接返回
   * @note 当构造函数的auto_reset为true时，Wait方法在被解除阻塞的同时，会立即自动重置notified_状态为false，
   *       使得下一次Wait调用能直接产生阻塞等待，否则需要先手动调用Reset方法重置notified_状态为false，才能在下一次Wait调用产生阻塞等待
   */
  void Wait();

  /**
   * @brief 当notified_状态为false时，进行阻塞式等待，直到出现Notify调用，或等到timeout时限；否则不阻塞，直接返回
   * @note 当构造函数的auto_reset为true时，Wait方法在被解除阻塞的同时，会立即自动重置notified_状态为false，
   *       使得下一次Wait调用能直接产生阻塞等待，否则需要先手动调用Reset方法重置notified_状态为false，才能在下一次Wait调用产生阻塞等待
   * @param timeout 最大等待时长
   * @return 返回true表示未发生Notify调用，已经等到timeout时限；返回false表示发生了Notify调用，导致提前结束等待
   */
  bool Wait(const TDuration& timeout);

  /**
   * @brief 立刻结束等待，同时标记notified_状态为true
   */
  void Notify();

  /**
   * @brief 返回当前的notified_状态值
   */
  bool IsNotified() const;

  /**
   * @brief 重置notified_状态为false
   */
  void Reset();

 private:
  bool auto_reset_;
  bool notified_ = false;
  mutable std::mutex mutex_;
  std::condition_variable cv_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
