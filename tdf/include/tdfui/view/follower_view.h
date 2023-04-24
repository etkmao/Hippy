// Copyright (c) 2021, Tencent Inc.
// All rights reserved.
#pragma once
#include "tdfui/view/render_object_view.h"
#include "core/engine/render/follower_object.h"
#include "tdfui/view/leader_view.h"

namespace tdfcore {

class FollowerView : public RenderObjectView {
 public:
  ~FollowerView() override = default;
  void SetLayerLink(const std::shared_ptr<LayerLink> &link);
  void SetFollowOffset(const TPoint &offset) { follower_->SetFollowOffset(offset); }

 protected:
  explicit FollowerView(const std::shared_ptr<LayerLink> &link);

 private:
  std::shared_ptr<FollowerObject> follower_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
