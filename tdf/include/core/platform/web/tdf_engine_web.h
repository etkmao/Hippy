//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <emscripten.h>
#include <emscripten/html5.h>
#include <memory>
#include "core/engine/schedule/signal_task_driver.h"
#include "core/platform/common/tdf_engine.h"
#include "core/platform/web/event/platform_event_handler_web.h"
#include "core/platform/web/platform_view_web.h"
#include "core/tdfi/shell.h"

namespace tdfcore {

class TDFEngineWeb final : public TDFEngine {
 public:
  ~TDFEngineWeb() override;
  explicit TDFEngineWeb();

  /**
   * 获取引擎Pipeline任务的TaskDriver
   * @return
   */
  std::shared_ptr<SignalTaskDriver> GetPipelineDriver() const;

  std::shared_ptr<PlatformEventHandler> GetEventDispatcher() const { return event_handler_; }

  void Init() override;

 private:
  std::shared_ptr<Platform> MakePlatform() override;

  std::shared_ptr<PlatformView> MakePlatformView() override;

  std::shared_ptr<TaskDriver> MakePipelineDriver() override;

  virtual std::shared_ptr<Scheduler> MakeScheduler() override;

  std::shared_ptr<PlatformEventHandler> event_handler_;
};

}  // namespace tdfcore
