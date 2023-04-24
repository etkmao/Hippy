//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <emscripten/val.h>
#include "core/platform/common/view/platform_view.h"

namespace tdfcore {

class PlatformViewWeb : public PlatformView {
 public:
  void SetFrame(const TRect &frame) override;
  TRect GetFrame() override;
  void InsertView(const std::shared_ptr<PlatformView> &platform_view, int64_t index) override;
  void RemoveView(const std::shared_ptr<PlatformView> &platform_view) override;
  void *GetViewPointer() override;
  void InitWindow(const std::shared_ptr<Device> &shared_device) override;
  const std::string &GetElementId() const { return element_id_; }
  bool IsReusable() const { return is_reusable_; }
  void SetReusable(bool is_reusable) { is_reusable_ = is_reusable; }

 protected:
  explicit PlatformViewWeb(const std::string &element_id,
                           const std::shared_ptr<Scheduler>& scheduler);

 private:
  static void UpdateReusableIfNeeded(const std::shared_ptr<PlatformView>& platform_view, bool is_reusable);
  std::string element_id_;
  emscripten::val canvas_holder_ = emscripten::val::null();
  bool is_reusable_ = false;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
