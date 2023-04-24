//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/platform/common/view/platform_embedded_view.h"
#include "core/platform/common/view/platform_view.h"

@class TDFChildClippingView;
@class EmbeddedHostView;
namespace tdfcore {

class PlatformEmbeddedViewIOS : public PlatformEmbeddedView {
 public:
  explicit PlatformEmbeddedViewIOS(EmbeddedHostView* view,
                                   const std::string &view_type,
                                   const PlatformEmbeddedView::PropsMap &props_map,
                                   const std::shared_ptr<ChannelRuntime>& channel_runtime);

  ~PlatformEmbeddedViewIOS() override;
  TRect GetFrame() override;
  void SetFrame(const TRect &frame) override;
  void InsertView(const std::shared_ptr<PlatformView> &platform_view, int64_t index) override;
  void RemoveView(const std::shared_ptr<PlatformView> &platform_view) override;
  void *GetViewPointer() override;
  void UpdateMutatorsToNative(const MutatorsStack &mutators_stack) override;
  void UpdateProps(const PropsMap &props) override;
  std::future<std::shared_ptr<Bitmap>> MakeSnapshot() override;

 protected:
  TDFChildClippingView *view_;
};

}  // namespace tdfcore
