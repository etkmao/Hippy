//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <string>
#include "core/platform/darwin/ios/view/TDFView.h"
#include "core/platform/common/view/platform_view.h"

namespace tdfcore {
class PlatformViewIOS final : public PlatformView {
 public:
  explicit PlatformViewIOS(TDFView *view, const std::shared_ptr<Scheduler> &scheduler);
  ~PlatformViewIOS() override;
  void SetFrame(const TRect &frame) override;
  tdfcore::TRect GetFrame() override;
  void NotifyFrameChange() override;
  void InsertView(const std::shared_ptr<PlatformView> &view, int64_t index) override;
  void RemoveView(const std::shared_ptr<PlatformView> &view) override;
  void *GetViewPointer() override;
  void InitWindow(const std::shared_ptr<Device> &shared_device) override;
  void Present(GraphicsContext *graphics_context) override;
 protected:
  void Init() override;
 private:
  TDFView *view_;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
