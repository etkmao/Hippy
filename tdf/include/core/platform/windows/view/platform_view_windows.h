//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/platform/common/view/platform_view.h"

namespace tdfcore {
class TDFView;
class PlatformViewWindows : public PlatformView {
 public:
  PlatformViewWindows(std::shared_ptr<TDFView> view, const std::shared_ptr<Scheduler> &scheduler);
  void SetFrame(const TRect &frame) override;
  TRect GetFrame() override;
  void InsertView(const std::shared_ptr<PlatformView> &view, int64_t index) override;
  void RemoveView(const std::shared_ptr<PlatformView> &view) override;
  void NotifyFrameChange() override;
  void *GetViewPointer() override;
  void InitWindow(const std::shared_ptr<Device> &shared_device) override;
  void Present(GraphicsContext *graphics_context) override;

 protected:
 private:
  std::shared_ptr<TDFView> view_;
  bool render_target_dirty_ = true;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
