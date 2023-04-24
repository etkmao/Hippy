//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <string>
#include <memory>
#include "core/platform/common/channel/channel_runtime.h"
#include "core/platform/windows/channel/channel_invoker.h"
#include "core/platform/common/channel/native_value_wrapper.h"


namespace tdfcore {

class ChannelRuntimeWindows : public ChannelRuntime {
 public:
  std::shared_ptr<HostObject> GetHostObject(const std::string &name) override;
};

}  // namespace tdfcore