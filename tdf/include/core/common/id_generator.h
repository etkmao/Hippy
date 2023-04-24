//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <atomic>
#include "core/common/macro.h"

namespace tdfcore {
class IDGenerator {
 public:
  static uint64_t NextUniqueID() {
    static std::atomic<uint64_t> nextID(1);
    uint64_t id;
    do {
      id = nextID.fetch_add(1);
    } while (id == 0);  // 0 is reserved for an invalid id.
    return id;
  }
};
}  // namespace tdfcore
