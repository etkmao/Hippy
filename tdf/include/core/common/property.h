//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <cstring>
#include <codecvt>
#include <locale>
#include <memory>
#include <sstream>
#include <string>
#include "core/common/object.h"

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#  define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define TYPE_PROPERTY_INSTANTIATION(TYPE)                    \
 public:                                                     \
  constexpr const TYPE &Value() const { return value_; }     \
  TypeProperty(const std::string &name, const TYPE &value)   \
    : value_(value) {                                        \
    name_ = name;                                            \
    type_ = GetTypeName<TYPE>();                             \
  }                                                          \
  virtual std::string ToString() const override {            \
    return name_ + " " + type_ + ": " + ValueToString();     \
  }                                                          \
                                                             \
 protected:                                                  \
  TYPE value_;

namespace tdfcore {

template <typename T>
inline const char *GetTypeName() {
  std::string prefix, suffix;
#if defined(_MSC_VER)
  prefix = "const char *__cdecl tdfcore::GetTypeName<";
  suffix = ">(void)";
#else
  prefix = "const char *tdfcore::GetTypeName() [T = ";
  suffix = "]";
#endif
  auto prefix_size = prefix.size();
  auto suffix_size = suffix.size();
  auto fn_size = strlen(__PRETTY_FUNCTION__);
  constexpr const char *function_name = __PRETTY_FUNCTION__;
  static const std::string name =
      std::string(function_name + prefix_size, fn_size - prefix_size - suffix_size);
  return name.c_str();
}

template <typename T>
class TypeProperty;

class Property : public Object {
 public:
  constexpr const std::string &Name() const { return name_; }

  constexpr const std::string &Type() const { return type_; }

  template <typename T>
  static std::shared_ptr<Property> Make(const std::string &name, const T &value) {
    return TDF_MAKE_SHARED(TypeProperty<T>, name, value);
  }

  ~Property() override = default;

  virtual std::string ValueToString() const = 0;

 protected:
  std::string name_;
  std::string type_;
};

template <typename T>
class TypeProperty : public Property {
 public:
  constexpr const T &Value() const { return value_; }

  TypeProperty(const std::string &name, const T &value) {
    name_ = name;
    value_ = value;
    type_ = GetTypeName<T>();
  }

  std::string ValueToString() const override {
    std::stringstream ss;
    ss << value_;
    return ss.str();
  }

  std::string ToString() const override {
    return name_ + " " + type_ + ": " + ValueToString();
  }

 protected:
  T value_;
};

template <>
class TypeProperty<std::u16string> : public Property {
TYPE_PROPERTY_INSTANTIATION(std::u16string)
  std::string ValueToString() const override {
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(value_);
  }
};
}  // namespace tdfcore
