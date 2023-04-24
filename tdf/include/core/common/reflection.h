/**
 * Copyright (c) 2020 Tencent Corporation. All rights reserved.
 * @brief 动态化框架反射实现
 * @details 主要基于静态反射和少量动态反射，对性能无影响
 */

#pragma once

#include <vector>
#include "core/common/object.h"
#include "core/common/property.h"
#include "core/common/refl.hpp"

/**
 * @brief 联合使用Reflectable接口、以及TDF_REFF开头的一系列宏，能够将一个普通类型自动升级成为带有反射能力的类型
 * @detail
 * 样例1：非模板类
 *   namespace tdfcore {
 *     class Foo : public Reflectable {
 *     TDF_REFF_CLASS_META(Foo)
 *      public:
 *       int a;
 *     };
 *   }
 *
 *   TDF_REFL_DEFINE(tdfcore::Foo, bases<>)
 *   TDF_REFL_MEMBER(a);
 *   TDF_REFL_END(tdfcore::Foo)
 *
 * 样例2：模板类
 *   namespace tdfcore {
 *     template<typename T>
 *     class Foo : public Reflectable {
 *     TDF_REFF_CLASS_META(Foo<T>)
 *      public:
 *       T a;
 *     };
 *   }
 *
 *   TDF_REFL_TEMPLATE_DEFINE((typename T), (tdfcore::Foo<T>), bases<>)
 *   TDF_REFL_MEMBER(a);
 *   TDF_REFL_END(tdfcore::Foo<T>)
 */

/**
 * @brief 请在那些继承了Reflectable接口的类型中添加此宏，用于自动生成反射类型与方法
 */
#define TDF_REFF_CLASS_META(class_name)                                                                      \
 public:                                                                                                     \
  friend struct ::refl_impl::metadata::type_info__<class_name>;                                              \
  static const ::tdfcore::Type &ClassType() { return ::tdfcore::Type::Get<class_name>(); }               \
  virtual const ::tdfcore::Type &GetType() const override { return ::tdfcore::Type::Get<class_name>(); } \
  static std::shared_ptr<::tdfcore::Reflection> ClassReflection() {                                        \
    return ::tdfcore::ReflectionImpl<class_name>::Make();                                                  \
  }                                                                                                          \
  virtual std::shared_ptr<::tdfcore::Reflection> GetReflection() const override {                          \
    return ::tdfcore::ReflectionImpl<class_name>::Make();                                                  \
  }

/**
 * @brief 以下宏用于声明反射类型，以及可以反射的成员变量
 * @detail 以下宏需要成套联合使用：
 *         首先以 TDF_REFL_DEFINE（非模板类）或 TDF_REFL_TEMPLATE_DEFINE（模板类）作为开头，声明反射类型
 *         然后以 TDF_REFL_MEMBER 声明该类型中可以反射的成员变量
 *         最终以 TDF_REFL_END 作为结尾
 */
#define TDF_REFL_DEFINE(TypeName, ...) REFL_TYPE(TypeName, __VA_ARGS__)
#define TDF_REFL_TEMPLATE_DEFINE(TemplateDeclaration, TypeName, ...) \
  REFL_TEMPLATE(TemplateDeclaration, TypeName, __VA_ARGS__)
#define TDF_REFL_MEMBER(FieldName_, ...) REFL_FIELD(FieldName_, __VA_ARGS__)
#define TDF_REFL_END(class_name) REFL_END

#define TDF_REFL_TYPE(TypeName, ...) \
  REFL_TYPE(TypeName, __VA_ARGS__) \
  REFL_END

// 【内部宏】反射类型
#define __TDF_REFF_CLASS__(class_name) ::refl::trait::remove_qualifiers_t<class_name>

namespace tdfcore {

class Reflectable;

/**
 * @brief 类型的描述信息
 * @detail 同一反射类型的Reflectable全体实例共享一个Type实例
 */
class Type {
 public:
  /**
   * @brief 获取类型描述信息
   * @return 类型描述信息。
   */
  template <typename T>
  static const Type &Get() {
    static const Type ti(refl::reflect<__TDF_REFF_CLASS__(T)>());
    return ti;
  }

  /**
   * @brief 获取类型信息中的类名
   * @return 类名的字符串。
   */
  const char *GetName() const { return name_; }

  bool operator==(const Type &type) const { return name_ == type.name_; }

  bool operator!=(const Type &type) const { return name_ != type.name_; }

  /**
   * @brief 主要是给std::set存储时使用
   */
  bool operator<(const Type &type) const { return strcmp(name_, type.name_); }

 private:
  const char *name_;

  template <typename T, typename... Fields>
  explicit Type(refl::type_descriptor<T> td) : name_(td.name) {}
};

/**
 * @brief 运行时的反射能力
 * @detail 每个Reflectable实例拥有一个Reflection实例。
 *         当Reflectable实例析构后，对应的Reflection实例将变为Expired状态，该状态下不可再使用任何反射能力
 */
class Reflection : public Object {
 public:
  ~Reflection() override = default;

  /**
   * @brief 获取指定的Reflectable实例的全体反射成员列表
   * @param instance 指定的Reflectable实例
   */
  virtual std::vector<std::shared_ptr<tdfcore::Property>> GetProperties(const Reflectable *instance) const = 0;

 protected:
  Reflection() = default;

  friend class Reflectable;
};

/**
 * @brief 【抽象类】运行时反射的接口
 */
class Reflectable {
 public:
  /**
   * @brief 获取类型
   */
  virtual const Type &GetType() const = 0;

  /**
   * @brief 获取反射能力
   */
  virtual std::shared_ptr<Reflection> GetReflection() const = 0;
};

/**
 * @brief 【内部类】Reflection的具体实现类
 * @detail 通过模板，保留实际的对象类型，以实现反射能力
 */
template <typename T>
class ReflectionImpl : public Reflection {
 public:
  static std::shared_ptr<ReflectionImpl> Make() { return TDF_MAKE_SHARED(ReflectionImpl<T>); }

  ~ReflectionImpl() override = default;

  std::vector<std::shared_ptr<tdfcore::Property>> GetProperties(const Reflectable *instance) const override {
    auto *t_instance = static_cast<const T *>(instance);
    assert(t_instance != nullptr);
    std::vector<std::shared_ptr<tdfcore::Property>> properties;
    refl::util::for_each(GetDescriptor().members, [&properties, t_instance](auto member) {
      properties.push_back(tdfcore::Property::Make(get_display_name(member), member(*t_instance)));
    });
    return properties;
  }

 protected:
  ReflectionImpl() : Reflection() {}

  inline refl::type_descriptor<__TDF_REFF_CLASS__(T)> GetDescriptor() const {
    return refl::reflect<__TDF_REFF_CLASS__(T)>();
  }

 private:
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
