//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <emscripten/val.h>
#include "core/platform/common/view/platform_embedded_view.h"
#include "core/platform/common/view/platform_view.h"
namespace tdfcore {

class PlatformEmbeddedViewWeb : public PlatformEmbeddedView {
 public:
  PlatformEmbeddedViewWeb(emscripten::val embedded_element, const std::string &view_type,
                          const PlatformEmbeddedView::PropsMap &props_map,
                          const std::shared_ptr<ChannelRuntime> &channel_runtime);
  TRect GetFrame() override;
  void SetFrame(const TRect &frame) override;
  void InsertView(const std::shared_ptr<PlatformView> &platform_view, int64_t index) override{};
  void RemoveView(const std::shared_ptr<PlatformView> &platform_view) override{};
  void *GetViewPointer() override;
  void UpdateMutatorsToNative(const MutatorsStack &mutators_stack) override;
  void UpdateProps(const PropsMap &props) override;
  std::future<std::shared_ptr<Bitmap>> MakeSnapshot() override;

 protected:
  /// 由使用者创建的htmlElement的JS对象
  emscripten::val embedded_element_ = emscripten::val::null();
  /// 由管理上述Element的JS对象
  emscripten::val embedded_element_holder_ = emscripten::val::null();
  std::string view_type_;
};

}  // namespace tdfcore
