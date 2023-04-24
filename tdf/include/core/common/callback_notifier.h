//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <mutex>
#include <unordered_map>
#include "core/common/object.h"

namespace tdfcore {

/**
 * @brief 基于std::function回调类型的通知器；模板类型Args即为回调函数的参数类型列表
 * @details
 *   模板特化举例：
 *     CallbackNotifier<> 对应的回调函数类型为 std::function<void()>
 *     CallbackNotifier<int, float> 对应的回调函数类型为 std::function<void(int, float)>
 *   参考案例：
 *     TaskRunner中的WakeUpNotifier
 */
template <typename... Args>
class CallbackNotifier : public Object {
 public:
  ~CallbackNotifier() override = default;

  /**
   * @brief 与本类对应的std::function回调类型
   */
  using Callback = std::function<void(Args...)>;

  /**
   * @brief 添加一个回调目标
   * @return 与该回调目标相对应的标识符
   */
  uint64_t Add(const Callback &callback) {
    assert(callback);
    std::lock_guard<std::mutex> lock(mutex_);
    auto callback_id = ++counter_;
    callbacks_[callback_id] = callback;
    return callback_id;
  }

  /**
   * @brief 删除一个回调目标
   * @param callback_id 与该回调目标相对应的标识符
   */
  void Remove(uint64_t callback_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    callbacks_.erase(callback_id);
  }

  /**
   * @brief 是否存在回调目标
   */
  bool Empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return callbacks_.empty();
  }

  /**
   * @brief 向回调目标发起调用；Notify的参数列表与Callback的参数列表一致
   */
  void Notify(Args &&...args) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto callbacks = callbacks_;
    lock.unlock();
    for (const auto &pair : callbacks) {
      pair.second(std::forward<Args>(args)...);
    }
  }

 private:
  std::mutex mutex_;
  std::unordered_map<uint64_t, Callback> callbacks_;
  uint64_t counter_ = 0;
};

}  // namespace tdfcore
