//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include "core/platform/common/channel/host_object.h"
#include "core/platform/common/channel/native_value_wrapper.h"
#include "core/platform/windows/channel/channel_invoker.h"

namespace tdfcore {
class HostObjectWindows final : public HostObject {
 public:
  explicit HostObjectWindows(const std::shared_ptr<ChannelInvoker> &channel);

  std::shared_ptr<NativeValueWrapper> InvokeMethod(const std::string &method,
                                                   const NativeValueWrapper arguments[],
                                                   size_t arg_count) override;

  constexpr const std::shared_ptr<ChannelInvoker> &GetChannelObject() { return channel_obj_; }

  void SetMethodHandler(const tdfcore::MethodHandler &handler) override;

  void RemoveMethodHandler() override;

 private:
  std::shared_ptr<ChannelInvoker> channel_obj_;
};
}  // namespace tdfcore
