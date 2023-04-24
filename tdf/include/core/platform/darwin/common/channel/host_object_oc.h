//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/platform/common/channel/host_object.h"
#include "core/platform/common/channel/native_value_wrapper.h"

namespace tdfcore {
class HostObjectOC final : public HostObject {
 public:
  explicit HostObjectOC(void *channel_obj, const std::string &name);

  std::shared_ptr<NativeValueWrapper> InvokeMethod(const std::string &method,
                                                   const NativeValueWrapper arguments[],
                                                   size_t arg_count) override;

  void SetMethodHandler( const MethodHandler &callback) override;

  void RemoveMethodHandler() override;

private:
  void InitPropertyMap();
  void *channel_obj_;
  std::string channel_name_;
};
}
