//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/android/channel/channel_utils.h"
#include "core/platform/common/channel/host_object.h"
#include "core/platform/common/channel/native_value_wrapper.h"

namespace tdfcore {

class HostObjectJava final : public HostObject {
 public:
  explicit HostObjectJava(JavaWeakGlobalRef<jobject> jobject_ref)
      : jobject_ref_(jobject_ref), ref_clazz_(nullptr) {
    InitPropertyMap();
  }

  std::shared_ptr<NativeValueWrapper> InvokeMethod(const std::string &method,
                                                   const NativeValueWrapper arguments[],
                                                   size_t arg_count) override;

  void SetMethodHandler(const tdfcore::MethodHandler &handler) override;

  void RemoveMethodHandler() override;

  std::shared_ptr<NativeValueWrapper> CallHandlerMethod(
      const std::string &method,
      const NativeValueWrapper arguments[],
      size_t arg_count);

  void InitPropertyMap();

  void DeleteGlobalRef(std::shared_ptr<JNIArgs> &jniArgs);

 private:
  JavaWeakGlobalRef<jobject> jobject_ref_;
  JavaWeakGlobalRef<jclass> ref_clazz_;
  // methodName, signature
  std::unordered_map<std::string, MethodInfo> method_cache_;
};

}  // namespace tdfcore
