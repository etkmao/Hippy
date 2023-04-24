//
// Copyright (c) 2021 com.tencent. All rights reserved.

//
#pragma once

#include <string>
#include <variant>
#include "core/common/object.h"
#include "core/platform/common/channel/byte_stream.h"

namespace tdfcore {

/**
 * 类型包装类，用于Java/OC类型与C++类型转换
 * 将支持基础类型、数组、集合、Java/OC对象，buffer等
 * */
class NativeValueWrapper final : public Object {
 public:
  using Callback = std::function<std::shared_ptr<NativeValueWrapper>(
      const std::string &method, const NativeValueWrapper arguments[], size_t arg_count)>;

  enum class Type {
    Undefined,
    Null,
    Boolean,
    Int32,
    Int64,
    Double,
    Float,
    U16String,
    Object,
    Callback,
    ByteStream,
  };

 public:
  NativeValueWrapper() : type_(Type::Undefined) {}
  explicit NativeValueWrapper(Type type) : type_(type) {}
  explicit NativeValueWrapper(int32_t int_value) : type_(Type::Int32), data_(int_value) {}
  explicit NativeValueWrapper(int64_t int_value) : type_(Type::Int64), data_(int_value) {}
  explicit NativeValueWrapper(double double_value) : type_(Type::Double), data_(double_value) {}
  explicit NativeValueWrapper(float float_value) : type_(Type::Float), data_(float_value) {}
  explicit NativeValueWrapper(bool bool_value) : type_(Type::Boolean), data_(bool_value) {}
  explicit NativeValueWrapper(const std::u16string &u16string_value) : type_(Type::U16String), data_(u16string_value) {}
  explicit NativeValueWrapper(void *object) : type_(Type::Object), data_(object) {}
  explicit NativeValueWrapper(const Callback &callback) : type_(Type::Callback), data_(callback) {}
  explicit NativeValueWrapper(const std::shared_ptr<ByteStream> &byte_stream)
      : type_(Type::ByteStream), data_(byte_stream) {}

 public:
  Type ValueType() const;
  int32_t Int32Value() const;
  int64_t Int64Value() const;
  bool BooleanValue() const;
  double DoubleValue() const;
  float FloatValue() const;
  std::u16string &U16StringValue();
  const std::u16string &U16StringValue() const;
  void *Object() const;
  Callback CallbackFunction() const;
  const std::shared_ptr<ByteStream> &ByteStreamValue() const;

 public:
  template <typename... Args>
  static std::shared_ptr<NativeValueWrapper> InvokeCallback(const Callback &callback, const std::string &method,
                                                            Args... args) {
    if (callback == nullptr) {
      return nullptr;
    }
    NativeValueWrapper arguments[] = {NativeValueWrapper(args)...};
    return callback(method, arguments, sizeof(arguments) / sizeof(NativeValueWrapper));
  }

  static std::shared_ptr<NativeValueWrapper> Null() {
    return TDF_MAKE_SHARED(NativeValueWrapper, NativeValueWrapper::Type::Null);
  }

 private:
  Type type_ = Type::Undefined;
  std::variant<int32_t, int64_t, double, float, bool, std::u16string, void *, Callback, std::shared_ptr<ByteStream>>
      data_;
};

using MethodHandler = std::function<std::shared_ptr<NativeValueWrapper>(
    const std::string &method, const NativeValueWrapper arguments[], size_t arg_count)>;

}  // namespace tdfcore
