//
// Copyright (c) 2021, Tencent Inc.
// All rights reserved.

#pragma once

#include "core/engine/render/render_object.h"
#include "tdfui/view/view.h"

namespace tdfcore {

class RenderObjectView : public tdfcore::View {
 public:
  ~RenderObjectView() override = default;
  void Init() override;
  std::shared_ptr<tdfcore::RenderObject> GetCustomRenderObject() const { return render_object_; }

 protected:
  enum RenderObjectIndex { kCustomRenderObject = View::kRenderObjectCount, kRenderObjectCount };
  explicit RenderObjectView(const std::shared_ptr<tdfcore::RenderObject>& render_object = nullptr,
                            const std::shared_ptr<ViewContext>& view_context = ViewContext::GetCurrent());
  int32_t GetTotalRenderObjectCount() override;

  std::shared_ptr<tdfcore::RenderObject> render_object_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
