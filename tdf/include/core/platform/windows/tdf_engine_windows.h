//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/platform/common/embedded_view_manager.h"
#include "core/platform/common/tdf_engine.h"
#include "core/platform/windows/embedded_view/embedded_view_register.h"
#include "core/platform/windows/event/platform_event_handler_windows.h"
#include "core/platform/windows/platform_windows.h"
#include "core/platform/windows/view/platform_view_windows.h"
#include "core/platform/windows/view/tdf_output_view.h"
#include "core/platform/windows/windows_define.h"
#include "core/tdfi/shell.h"

namespace tdfcore {

/**
 * @brief 用于初始化shell的引擎
 */
class TDFEngineWindows final : public TDFEngine, public std::enable_shared_from_this<TDFEngineWindows> {
 public:
  explicit TDFEngineWindows(const TDFEngineConfig& config);

  /**
   * @brief 窗口菜单点击消息
   */
  void OnMenuCommand(uint16_t command_id);

  std::shared_ptr<TDFOutputView> GetOutputView() const { return output_view_; }

  std::shared_ptr<EmbeddedViewRegister> GetEmbeddedViewRegister() const { return embedded_view_register_; }

 protected:
  void Init() override;

  std::shared_ptr<Platform> MakePlatform() override;

  std::shared_ptr<PlatformView> MakePlatformView() override;

  std::shared_ptr<TaskDriver> MakePipelineDriver() override;

 private:
  void SetFallbackFonts();
  std::shared_ptr<TDFOutputView> output_view_;
  std::shared_ptr<EmbeddedViewRegister> embedded_view_register_;
  std::shared_ptr<PlatformEventHandler> event_handler_;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
