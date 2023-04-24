//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <functional>
#include <memory>
#include "core/common/macro.h"

/**
 * @brief 对继承自tdfcore::Object的类型，统一实现make_shared、make_unique等智能指针构造函数
 * @detail
 *   凡继承自tdfcore::Object的类:
 *   （1）请在该类的定义体内追加FRIEND_OF_TDF_ALLOC
 *   （2）统一使用TDF_MAKE_SHARED（_WITH_DELETER）构造shared_ptr实例
 *   （3）统一使用TDF_MAKE_UNIQUE（_WITH_DELETER）构造unique_ptr实例
 */

#define TDF_MAKE_SHARED(type, ...) ::tdfcore::MakeShared<type>(__FILE__, __LINE__, #type, ##__VA_ARGS__)
#define TDF_MAKE_SHARED_WITH_DELETER(type, deleter, ...) \
  ::tdfcore::MakeSharedWithDeleter<type>(__FILE__, __LINE__, #type, deleter, ##__VA_ARGS__)

#define TDF_MAKE_UNIQUE(type, ...) ::tdfcore::MakeUnique<type>(__FILE__, __LINE__, #type, ##__VA_ARGS__)
#define TDF_MAKE_UNIQUE_WITH_DELETER(type, deleter_type, ...) \
  ::tdfcore::MakeUniqueWithDeleter<type, deleter_type>(__FILE__, __LINE__, #type, ##__VA_ARGS__)

/**
 * @brief 为Object增加友元函数；与TDF_MAKE_SHARED等宏配套使用
 */
#define FRIEND_OF_TDF_ALLOC                                                                                       \
  template <class _T, typename... _Args>                                                                          \
  friend inline _T * ::tdfcore::ConstructObject(const char *file, int line, const char *type, _Args &&...__args); \
  template <class _T>                                                                                             \
  friend inline void ::tdfcore::InitObject(_T *obj);

/**
 * @note 以下为上述宏定义所涉及到的具体实现，请勿直接使用
 */
namespace tdfcore {

class Object;

/**
 * @brief 创建Object实例，并追踪内存分配（当Profile开启时）
 */
template <typename _T, typename... _Args>
inline _T *ConstructObject(const char *file, int line, const char *type, _Args &&...__args) {
  auto object = new _T(std::forward<_Args>(__args)...);
  object->Trace(type, sizeof(_T), file, line);
  return object;
}

/**
 * @brief 调用Object的Init()方法
 */
template <typename _T>
inline void InitObject(_T *object) {
  object->Init();
}

/**
 * @brief 创建一个shared_ptr<T>，并调用Init方法
 */
template <typename T, typename... Args>
typename std::enable_if<std::is_base_of<::tdfcore::Object, T>::value, std::shared_ptr<T>>::type
MakeShared(const char *file, int line, const char *type, Args &&...args) {
  auto object = ConstructObject<T>(file, line, type, std::forward<Args>(args)...);
  auto ptr = std::shared_ptr<T>(object);
  InitObject(object);
  return ptr;
}

/**
 * @brief 创建一个shared_ptr<T>（包含自定义的deleter），并调用Init方法
 */
template <typename T, typename D, typename... Args>
typename std::enable_if<std::is_base_of<::tdfcore::Object, T>::value, std::shared_ptr<T>>::type
MakeSharedWithDeleter(const char *file, int line, const char *type, D &&deleter, Args &&...args) {
  auto object = ConstructObject<T>(file, line, type, std::forward<Args>(args)...);
  auto ptr = std::shared_ptr<T>(object, deleter);
  InitObject(object);
  return ptr;
}

/**
 * @brief 创建一个unique_ptr<T>，并调用Init方法
 */
template <typename T, typename... Args>
typename std::enable_if<std::is_base_of<::tdfcore::Object, T>::value, std::unique_ptr<T>>::type
MakeUnique(const char *file, int line, const char *type, Args &&...args) {
  auto object = ConstructObject<T>(file, line, type, std::forward<Args>(args)...);
  auto ptr = std::unique_ptr<T>(object);
  InitObject(object);
  return ptr;
}

/**
 * @brief 创建一个unique_ptr<T, D>（包含自定义的deleter），并调用Init方法
 */
template <typename T, typename D, typename... Args>
typename std::enable_if<std::is_base_of<::tdfcore::Object, T>::value, std::unique_ptr<T, D>>::type
MakeUniqueWithDeleter(const char *file, int line, const char *type, Args &&...args) {
  auto object = ConstructObject<T>(file, line, type, std::forward<Args>(args)...);
  auto ptr = std::unique_ptr<T, D>(object);
  InitObject(object);
  return ptr;
}

}  // namespace tdfcore
