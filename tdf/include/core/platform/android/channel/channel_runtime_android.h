//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <string>
#include <unordered_map>
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/common/channel/native_value_wrapper.h"
#include "core/platform/common/channel/channel_runtime.h"

namespace tdfcore {
class ChannelRuntimeAndroid : public ChannelRuntime {
 public:
  explicit ChannelRuntimeAndroid(const JavaWeakGlobalRef<jobject>& channel_manager_ref);

  std::shared_ptr<HostObject> GetHostObject(const std::string &name) override;

 private:
  JavaWeakGlobalRef<jobject> channel_manager_ref_;
};
}  // namespace tdfcore
