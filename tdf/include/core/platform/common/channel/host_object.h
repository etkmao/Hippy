//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include "core/platform/common/channel/native_value_wrapper.h"

namespace tdfcore {

class HostObject : public Object {
 public:
  virtual std::shared_ptr<NativeValueWrapper> InvokeMethod(const std::string &method,
                                                           const NativeValueWrapper arguments[],
                                                           size_t arg_count);

  virtual void SetMethodHandler(const tdfcore::MethodHandler &handler);

  virtual void RemoveMethodHandler();

 protected:
  MethodHandler method_handler_;
};
}  // namespace tdfcore
