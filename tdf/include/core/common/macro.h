//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#define WEAK_THIS weak_this = weak_from_this()
#define SHARED_THIS self = this->shared_from_this()

// 表明该lambda不会被存储，可以安全使用this
#define THIS_NO_STORE this
#define HAS_SELF(type) auto self = std::static_pointer_cast<type>(weak_this.lock())
#define DEFINE_SELF(type) HAS_SELF(type);
#define DEFINE_AND_CHECK_SELF(type) \
  DEFINE_SELF(type)                 \
  if (!self) {                      \
    return;                         \
  }

#define DEFINE_MAKE_FUNCTION(type) \
  static std::shared_ptr<type> Make() { return TDF_MAKE_SHARED(type); }

/**
 * @brief 【代码规范】符号导出宏：适用于struct, class、function类型。
 *        1.明确知道符号需要导出到外部使用，使用 TDF_EXPORT 。
 *        2.明确知道符号不需要导出到外部使用，使用 TDF_HIDDEN 。
 *        3.否则使用 TDF_API 可开关。
 * @detail
 * 样例1：类
 *   class TDF_API xxx {}
 *
 * 样例2：结构体
 *   struct TDF_API xxx {}
 *
 * 样例3：函数
 *   TDF_API void xxx();
 */

#define TDF_STR_(STR) #STR
#define TDF_STR(STR) TDF_STR_(STR)
#define TDF_COMBINE_INNER(A, B) A##B
#define TDF_COMBINE(A, B) TDF_COMBINE_INNER(A, B)


#if __ANDROID__
#  define TDF_PLATFORM_ANDROID 1
#  define TDF_PLATFORM_MOBILE 1
#elif __APPLE__
#  include <TargetConditionals.h>
#  if TARGET_OS_IPHONE
#    define TDF_PLATFORM_IOS 1
#    define TDF_PLATFORM_MOBILE 1
#  elif TARGET_OS_MAC
#    define TDF_PLATFORM_MACOS 1
#    define TDF_PLATFORM_PC 1
#  else
#    error "Unknow Operator System"
#  endif
#elif defined(_WIN32)
#  define TDF_PLATFORM_WINDOWS 1
#  define TDF_PLATFORM_PC 1
#elif __linux__
#  define TDF_PLATFORM_LINUX 1
#  define TDF_PLATFORM_PC 1
#elif __EMSCRIPTEN__
#  define TDF_PLATFORM_WEB 1
#else
#  error "Unknow Operator System"
#endif

// Windows下DLL/.so exports.
#if !defined(TDF_IMPLEMENTATION)
#  define TDF_IMPLEMENTATION 0
#endif

// 符号导出开关
#if !defined(TDF_EXPORT_SYMBOL)
#  define TDF_EXPORT_SYMBOL 1
#endif

// 符号导出
#if !defined(TDF_EXPORT)
#  if defined(_MSC_VER)
// TODO(fermichen): Win暂未适配，TDF_EXPORT先为空
#    define TDF_EXPORT
#  else
#    define TDF_EXPORT __attribute__((visibility("default")))
#  endif
#endif

// 符号隐藏
#if !defined(TDF_HIDDEN)
#  if defined(_MSC_VER)
#    define TDF_HIDDEN
#  else
#    define TDF_HIDDEN __attribute__((visibility("hidden")))
#  endif
#endif

// 默认使用
#if !defined(TDF_API)
#  if TDF_EXPORT_SYMBOL
#    if defined(_MSC_VER)
#      define TDF_API TDF_HIDDEN
#    else
#      define TDF_API TDF_EXPORT
#    endif
#  else
#    define TDF_API TDF_HIDDEN
#  endif
#endif

#if defined(__GNUC__)
#  define TDF_FORCE_INLINE  __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#  define TDF_FORCE_INLINE __forceinline
#else
#  define TDF_FORCE_INLINE inline
#endif

#if defined(__GNUC__)
#  define TDF_NO_INLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#  define TDF_NO_INLINE __declspec(noinline)
#else
#  define TDF_NO_INLINE
#endif

#if defined(__clang__) || defined(__GNUC__)
#  define TDF_CPP_STANDARD __cplusplus
#elif defined(_MSC_VER)
#  define TDF_CPP_STANDARD _MSVC_LANG
#endif

#if CPP_STANDARD >= 199711L
#  define TDF_HAS_CPP_03 1
#endif
#if CPP_STANDARD >= 201103L
#  define TDF_HAS_CPP_11 1
#endif
#if CPP_STANDARD >= 201402L
#  define TDF_HAS_CPP_14 1
#endif
#if CPP_STANDARD >= 201703L
#  define TDF_HAS_CPP_17 1
#endif

#if defined(__clang__)
#  define TDF_COMPILER_CLANG
#elif defined(__GNUC__)
#  define TDF_COMPILER_GCC
#elif defined(_MSC_VER)
#  define TDF_COMPILER_MSVC
#else
#  error "Unknow compiler"
#endif

#define TDF_EMBEDDER_ONLY

#define TDF_DISALLOW_COPY(TypeName) TypeName(const TypeName&) = delete

#define TDF_DISALLOW_ASSIGN(TypeName) TypeName& operator=(const TypeName&) = delete

#define TDF_DISALLOW_MOVE(TypeName) \
  TypeName(TypeName&&) = delete;         \
  TypeName& operator=(TypeName&&) = delete

#define TDF_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;               \
  TypeName& operator=(const TypeName&) = delete

#define TDF_DISALLOW_COPY_ASSIGN_AND_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;                    \
  TypeName(TypeName&&) = delete;                         \
  TypeName& operator=(const TypeName&) = delete;         \
  TypeName& operator=(TypeName&&) = delete

#define TDF_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName() = delete;                                    \
  TDF_DISALLOW_COPY_ASSIGN_AND_MOVE(TypeName)

#ifdef NDEBUG
#  define assert_fn(fn) ((void)0)
#else
#  define assert_fn(fn) \
    do {                \
      auto b = fn();    \
      assert(b);        \
    } while (0)
#endif

#define WEAK_CALL(weak, type, fn, ...) \
do {                              \
  auto shared_this = std::static_pointer_cast<type>(weak.lock()); \
  if (shared_this != nullptr) {   \
    shared_this->fn(__VA_ARGS__); \
  }\
} while (0)

// Emscripten符号导出
#ifndef TDF_EMSCRIPTEN_API
#  if defined(__EMSCRIPTEN__)
#    include <emscripten.h>
#    if defined(__cplusplus)
#      define TDF_EMSCRIPTEN_API(rettype) extern "C" rettype EMSCRIPTEN_KEEPALIVE
#    else
#      define TDF_EMSCRIPTEN_API(rettype) rettype EMSCRIPTEN_KEEPALIVE
#    endif
#  else
#    if defined(__cplusplus)
#      define TDF_EMSCRIPTEN_API(rettype) extern "C" rettype
#    else
#      define TDF_EMSCRIPTEN_API(rettype) rettype
#    endif
#  endif
#endif
