//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/platform/common/view/platform_embedded_view.h"
#include "core/platform/common/view/platform_view.h"
#include "core/platform/windows/embedded_view/embedded_host_view.h"

namespace tdfcore {

class PlatformEmbeddedViewWindows : public PlatformEmbeddedView {
 public:
  PlatformEmbeddedViewWindows(const std::shared_ptr<EmbeddedHostView> &view, const std::string &view_type,
                              const PlatformEmbeddedView::PropsMap &props_map,
                              const std::shared_ptr<ChannelRuntime> &channel_runtime);

  ~PlatformEmbeddedViewWindows();
  TRect GetFrame() override;
  void SetFrame(const TRect &frame) override;
  void InsertView(const std::shared_ptr<PlatformView> &platform_view, int64_t index) override;
  void RemoveView(const std::shared_ptr<PlatformView> &platform_view) override;
  void *GetViewPointer() override;
  void UpdateMutatorsToNative(const MutatorsStack &mutators_stack) override;
  void UpdateProps(const PropsMap &props) override;
  std::future<std::shared_ptr<Bitmap>> MakeSnapshot() override;

 private:
  std::shared_ptr<EmbeddedHostView> view_;
};

}  // namespace tdfcore
