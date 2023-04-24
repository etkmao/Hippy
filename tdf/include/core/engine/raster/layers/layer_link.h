//

//
// Copyright (c) Tencent Corporation. All rights reserved.
//
//

#pragma once

#include <memory>
#include "core/common/memory.h"
#include "core/common/object.h"

namespace tdfcore {
class LeaderLayer;

class LayerLink : public Object {
 public:
  ~LayerLink() override = default;
  bool IsValid() const { return !leader_.expired(); }
  void Reset();
  void SetLeader(const std::shared_ptr<LeaderLayer>& layer);
  std::shared_ptr<LeaderLayer> GetLeader() const;

 protected:
  LayerLink() = default;

 private:
  std::weak_ptr<LeaderLayer> leader_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
