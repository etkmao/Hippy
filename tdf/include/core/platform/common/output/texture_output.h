//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/platform/common/output/output.h"
#include "core/platform/common/platform.h"

namespace tdfcore {
/**
 * @brief 输出到纹理
 */
class TextureOutput : public Output {
 public:
  ~TextureOutput() override = default;

  /**
   * @brief 设置TextureOutput渲染目标，外部使用TextureOutput的开发者可以直接通过此接口设置想要的渲染目标（Texture）
   * @param texture 目标纹理
   * @param id 默认没有id参数，则是实则对应的 main_texture_，否则为 overlay_textures_
   */
  void SetTexture(const std::shared_ptr<Texture> &texture, std::optional<int64_t> id = std::nullopt);

  std::shared_ptr<Texture> GetTexture(std::optional<int64_t> id = std::nullopt) const;

  std::shared_ptr<Board> MakeBoard() override;

  bool IsValid() override;

  /**
   * @brief 使用 SetTexture
   */
  void InitBackend() override;

  void PresentMainView() override;

  void PresentOverlayViews() override;

  std::shared_ptr<Device> GetDevice() const override { return resource_device_; }

  void UpdateDevice(const std::shared_ptr<Device>& device) override;

 protected:
  TextureOutput(const std::shared_ptr<Scheduler> &scheduler,
                const std::shared_ptr<EmbeddedViewManager> &embedded_view_manager,
                const std::shared_ptr<Device> &shared_device);
  std::shared_ptr<Device> resource_device_;
  std::shared_ptr<Texture> main_texture_;
  std::unordered_map<int64_t, std::shared_ptr<Texture>> overlay_textures_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
