//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <cassert>
#include <functional>
#include <unordered_map>
#include "core/common/id_generator.h"

namespace tdfcore {
template<class... Type>
class Listener {
 public:
  uint64_t Add(std::function<void(Type...)> listener) {
    auto id = IDGenerator::NextUniqueID();
    listeners_[id] = listener;
    return id;
  }

  void Remove(uint64_t id) {
    assert(listeners_.find(id) != listeners_.end());
    listeners_.erase(id);
  }

  void Notify(Type... args) {
    for (const auto &listener : listeners_) {
      listener.second(args...);
    }
  }

 private:
  std::unordered_map<uint64_t, std::function<void(Type... args)>> listeners_;
};

// 特例，只负责接收通知，不需要信息（参数）的监听
using NoArgListener = Listener<>;
}  // namespace tdfcore
