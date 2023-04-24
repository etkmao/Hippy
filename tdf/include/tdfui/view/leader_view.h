// Copyright (c) 2021, Tencent Inc.
// All rights reserved.
#pragma once
#include "tdfui/view/render_object_view.h"
#include "core/engine/render/leader_object.h"

namespace tdfcore {

class LeaderView : public RenderObjectView {
 public:
  ~LeaderView() override = default;
  std::shared_ptr<LayerLink> GetLayerLink() const { return link_; }

 protected:
  LeaderView();

 private:
  std::shared_ptr<LayerLink> link_;
  std::shared_ptr<LeaderObject> leader_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
