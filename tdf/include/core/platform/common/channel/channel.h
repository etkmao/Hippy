//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <string>
#include "core/common/logging.h"
#include "core/platform/common/channel/channel_runtime.h"
#include "core/platform/common/channel/host_object.h"

namespace tdfcore {

class Channel : public Object {
 public:
  template <typename... Args>
  std::shared_ptr<NativeValueWrapper> InvokeMethod(const std::string &method, Args... args) {
    if (host_object_ == nullptr) {
      TDF_LOGE << "Channel InvokeMethod error, " << name_ << " channel not register.";
      // TODO(vimerzhao): remote this macro after support web's clipbord etc.
#ifndef __EMSCRIPTEN__
      assert(false);
#endif
      return nullptr;
    }

    NativeValueWrapper arguments[] = {NativeValueWrapper(args)...};
    return host_object_->InvokeMethod(method, arguments, sizeof...(args));
  }

  std::shared_ptr<NativeValueWrapper> InvokeMethod(const std::string &method) {
    if (host_object_ == nullptr) {
      TDF_LOGE << "Channel InvokeMethod error, " << name_ << " channel not register.";
      assert(false);
      return nullptr;
    }

    return host_object_->InvokeMethod(method, nullptr, 0);
  }

  void SetMethodHandler(const MethodHandler &method_handler);

 protected:
  /***
   * @brief Channel构造函数
   * @param name Channel对于的注册字符串，需要注册的Native对象对应
   */
  explicit Channel(const std::string &name);
  /****
   * @brief Channel构造函数
   * @param channel_runtime Channel运行时，如果当前环境存在多个ViewContext，则需要给到对应ViewContext的ChannelRuntime
   * @param name 注册字符串Key
   */
  explicit Channel(const std::shared_ptr<ChannelRuntime>& channel_runtime, const std::string &name);

 private:
  std::string name_;
  std::shared_ptr<HostObject> host_object_;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
