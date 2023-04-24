//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <functional>
#include <list>
#include <memory>
#include <utility>
#include <unordered_map>
#include <atomic>
#include "core/common/macro.h"

namespace tdfcore {
template<typename T>
class ValueNotifier {
 public:
  using Callback = std::function<void(const T&)>;
  static constexpr uint64_t kInvalidateCallback = 0;
  explicit ValueNotifier(T value = 0) : value_(std::move(value)) {}
  constexpr const T &GetValue() const { return value_; }

  void SetValue(T newValue) {
    if (value_ == newValue) {
      return;
    }
    value_ = newValue;
    NotifyListeners();
  }

  ~ValueNotifier() = default;

  virtual uint64_t AddListener(Callback listener) {
    static std::atomic<uint64_t> next(1);
    uint64_t handler;
    do {
      handler = next.fetch_add(1);
    } while (handler == 0);
    listeners_[handler] = std::move(listener);
    return handler;
  }

  virtual void RemoveListener(uint64_t handler) {
    auto f = listeners_.find(handler);
    if (f == listeners_.end()) {
      return;
    }
    listeners_.erase(f);
  }

  bool HasListeners() { return !listeners_.empty(); }

  void NotifyListeners() const {
    for (const auto &listener : listeners_) {
      listener.second(value_);
    }
  }

  virtual void Dispose() {
    listeners_.clear();
  }

 private:
  std::unordered_map<uint64_t, Callback> listeners_;
  T value_;
};
}  // namespace tdfcore
