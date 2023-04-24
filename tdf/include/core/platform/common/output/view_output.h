//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <vector>
#include "core/engine/schedule/scheduler.h"
#include "core/platform/common/embedded_view_manager.h"
#include "core/engine/graphics/board.h"
#include "core/platform/common/output/output.h"
#include "core/platform/common/platform.h"
#include "core/engine/graphics/texture.h"
#include "core/platform/common/view/platform_view.h"

namespace tdfcore {
/**
 * @brief 输出到窗口
 */
class ViewOutput : public Output {
 public:
  ~ViewOutput() override = default;

  std::shared_ptr<Board> MakeBoard() override;

  bool IsValid() override;
  void InitBackend() override;

  void PresentMainView() override;

  void PresentOverlayViews() override;

  std::shared_ptr<Device> GetDevice() const override { return main_platform_view_->GetDevice(); }

  void UpdateDevice(const std::shared_ptr<Device>& device) override;

 protected:
  ViewOutput(const std::shared_ptr<Scheduler> &scheduler,
             const std::shared_ptr<EmbeddedViewManager> &embedded_view_manager,
             const std::shared_ptr<Device> &shared_device,
             const std::shared_ptr<PlatformView> &platform_view);

  std::shared_ptr<Device> shared_device_;
  std::shared_ptr<PlatformView> main_platform_view_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
