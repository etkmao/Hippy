//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "core/common/object.h"
#include "core/platform/common/channel/native_value_wrapper.h"

class ChannelInvoker : public tdfcore::Object {
 public:
  ~ChannelInvoker() override = default;

  virtual std::shared_ptr <tdfcore::NativeValueWrapper>
  InvokeNativeMethod(const std::string &method,
                     const tdfcore::NativeValueWrapper arguments[],
                     size_t arg_count) = 0;

  template<typename... Args>
  std::shared_ptr <tdfcore::NativeValueWrapper> InvokeCoreMethod(const std::string &method,
                                                                 Args... args) {
    if (method_handler_) {
      NativeValueWrapper arguments[] = {NativeValueWrapper(args)...};
      std::u16string str = arguments[1].U16StringValue();
      return method_handler_(method, arguments, sizeof(arguments) / sizeof(NativeValueWrapper));
    }
    return std::shared_ptr<tdfcore::NativeValueWrapper>();
  }

  void SetMethodHandler(const tdfcore::MethodHandler &handler);

  void RemoveMethodHandler();

 private:
  tdfcore::MethodHandler method_handler_;
};
