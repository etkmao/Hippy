//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <vector>
#include "core/engine/schedule/scheduler.h"
#include "core/platform/common/embedded_view_manager.h"
#include "core/engine/graphics/board.h"
#include "core/platform/common/output/output.h"
#include "core/platform/common/output/view_output.h"
#include "core/platform/common/output/texture_output.h"
#include "core/platform/common/platform.h"
#include "core/platform/common/view/platform_view.h"

namespace tdfcore {
/**
 * @brief 先渲染到纹理，再渲染到ViewOutput
 */
class TextureToViewOutput : public Output {
 public:
  ~TextureToViewOutput() override = default;

  std::shared_ptr<Board> MakeBoard() override;

  bool IsValid() override;
  void InitBackend() override;
  void PresentMainView() override;
  void PresentOverlayViews() override;

  std::shared_ptr<Device> GetDevice() const override { return texture_output_->GetDevice(); }

  void UpdateDevice(const std::shared_ptr<Device>& device) override;

 protected:
  TextureToViewOutput(const std::shared_ptr<Scheduler> &scheduler,
                      const std::shared_ptr<EmbeddedViewManager> &embedded_view_manager,
                      const std::shared_ptr<Device> &resource_device,
                      const std::shared_ptr<PlatformView> &platform_view);

  void PrepareTexture();

  std::shared_ptr<ViewOutput> view_output_;
  std::shared_ptr<TextureOutput> texture_output_;
  std::shared_ptr<Board> view_board_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
