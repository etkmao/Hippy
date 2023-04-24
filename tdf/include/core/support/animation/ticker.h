//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include "core/common/time.h"
#include "core/tdfi/shell.h"
#include "core/tdfi/event_center.h"

namespace tdfcore {

using TickerCallback = std::function<void(TDuration)>;

class Ticker : public Object, public std::enable_shared_from_this<Ticker> {
 public:
  ~Ticker() override;

  explicit Ticker(const TickerCallback &ticker_callback, const std::shared_ptr<Shell> &shell = Shell::GetCurrent());

  bool Active() const;

  void Start();

  void Stop(bool canceled = true);

 private:
  void Schedule();

  std::optional<uint64_t> listener_id_;
  bool valid_ = true;
  bool active_ = false;
  TimePoint start_time_;
  TickerCallback on_tick_;
  std::shared_ptr<EngineClock> engine_clock_;
  std::shared_ptr<EventCenter> event_center_;
  std::shared_ptr<TaskRunner> ui_task_runner_;
};

}  // namespace tdfcore
