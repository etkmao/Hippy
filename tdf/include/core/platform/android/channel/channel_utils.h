//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <string>
#include <unordered_map>

#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/common/channel/native_value_wrapper.h"

namespace tdfcore {

struct JNIArgs {
  explicit JNIArgs(size_t count) : args_(count) {}

  std::vector<jvalue> args_;
  std::vector<jobject> globalRefs_;
};

struct MethodInfo {
  std::string signature_;
  jmethodID methodId_ = nullptr;
};

struct FieldInfo {
  std::string signature_;
  jfieldID fieldId_ = nullptr;
};

class ChannelUtils {
 public:
  static void InitChannelClazz();

  static std::unordered_map<std::string, MethodInfo> GetMethodMap(const std::string &methodMapStr);

  static std::vector<std::string> GetMethodArgTypesFromSignature(const std::string &methodSignature);

  static std::shared_ptr<JNIArgs> ConvertArgsToJNIArgs(const std::vector<std::string> &methodArgTypes,
                                                       const NativeValueWrapper arguments[],
                                                       size_t argCount);

  static std::shared_ptr<NativeValueWrapper> CallNativeMethod(const jobject &obj,
                                                              const MethodInfo &methodInfo,
                                                              const jvalue *args);

  static bool HandleBasicType(const std::string &type,
                              jvalue &jargs,
                              const NativeValueWrapper &value_wrapper);

  static bool HandleObjectType(const std::string &type,
                               jvalue &jargs,
                               const NativeValueWrapper &value_wrapper,
                               std::vector<jobject> &globalRefs);
};


static JavaGlobalRef<jclass> *g_integer_clazz;
static JavaGlobalRef<jclass> *g_double_clazz;
static JavaGlobalRef<jclass> *g_float_clazz;
static JavaGlobalRef<jclass> *g_long_clazz;
static JavaGlobalRef<jclass> *g_boolean_clazz;
static jmethodID g_integer_constructor;
static jmethodID g_double_constructor;
static jmethodID g_double_value;
static jmethodID g_float_constructor;
static jmethodID g_long_constructor;
static jmethodID g_boolean_constructor;
static jmethodID g_boolean_value;

const char kint[] = "I";
const char kdouble[] = "D";
const char kfloat[] = "F";
const char klong[] = "J";
const char kboolean[] = "Z";
const char kInteger[] = "Ljava/lang/Integer;";
const char kDouble[] = "Ljava/lang/Double;";
const char kFloat[] = "Ljava/lang/Float;";
const char kLong[] = "Ljava/lang/Long;";
const char kString[] = "Ljava/lang/String;";
const char kBoolean[] = "Ljava/lang/Boolean;";
const char kvoid[] = "V";
const char kBytes[] = "[B";
const char kUnSupportedType[] = "Lcom/invalid;";

}  // namespace tdfcore
