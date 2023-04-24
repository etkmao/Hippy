//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <string>
#include "core/platform/common/channel/native_value_wrapper.h"
#include "core/platform/common/channel/channel_runtime.h"
#include "core/platform/darwin/common/channel/ChannelRegistry.h"

namespace tdfcore {

class ChannelRuntimeDarwin : public ChannelRuntime {
 public:
  explicit ChannelRuntimeDarwin(ChannelRegistry *registry) : channel_registry_(registry) {} ;

  ChannelRegistry *GetChannelRegistry();

  std::shared_ptr<HostObject> GetHostObject(const std::string &name) override;

 private:
  ChannelRegistry *channel_registry_;
};

}  // namespace tdfcore
