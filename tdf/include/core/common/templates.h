//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <string.h>
#include <array>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace tdfnonstd {

// The name 'copy' here is fraught with peril. In this case it means 'append', not 'overwrite'.
// Alternate proposed names are 'propagate', 'augment', or 'append' (and 'add', but already taken).
// std::experimental::propagate_const already exists for other purposes in TSv2.
// These also follow the <dest, source> pattern used by boost.
template <typename D, typename S>
struct copy_const {
  using type = std::conditional_t<std::is_const<S>::value, std::add_const_t<D>, D>;
};
template <typename D, typename S>
using copy_const_t = typename copy_const<D, S>::type;

template <typename D, typename S>
struct copy_volatile {
  using type = std::conditional_t<std::is_volatile<S>::value, std::add_volatile_t<D>, D>;
};
template <typename D, typename S>
using copy_volatile_t = typename copy_volatile<D, S>::type;

template <typename D, typename S>
struct copy_cv {
  using type = copy_volatile_t<copy_const_t<D, S>, S>;
};
template <typename D, typename S>
using copy_cv_t = typename copy_cv<D, S>::type;

// The name 'same' here means 'overwrite'.
// Alternate proposed names are 'replace', 'transfer', or 'qualify_from'.
// same_xxx<D, S> can be written as copy_xxx<remove_xxx_t<D>, S>
template <typename D, typename S>
using same_const = copy_const<std::remove_const_t<D>, S>;
template <typename D, typename S>
using same_const_t = typename same_const<D, S>::type;
template <typename D, typename S>
using same_volatile = copy_volatile<std::remove_volatile_t<D>, S>;
template <typename D, typename S>
using same_volatile_t = typename same_volatile<D, S>::type;
template <typename D, typename S>
using same_cv = copy_cv<std::remove_cv_t<D>, S>;
template <typename D, typename S>
using same_cv_t = typename same_cv<D, S>::type;

}  // namespace tdfnonstd

namespace tdfcore {
/**
 *  Marks a local variable as known to be unused (to avoid warnings).
 *  Note that this does *not* prevent the local variable from being optimized away.
 */
template <typename T>
inline void tdf_ignore_unused_variable(const T&) {}

/**
 *  Returns a pointer to a D which comes immediately after S[count].
 */
template <typename D, typename S>
inline D* PtrAfter(S* ptr, size_t count = 1) {
  return reinterpret_cast<D*>(ptr + count);
}

/**
 *  Returns a pointer to a D which comes byteOffset bytes after S.
 */
template <typename D, typename S>
inline D* PtrAddOffset(S* ptr, ptrdiff_t byteOffset) {
  // The intermediate char* has the same cv-ness as D as this produces better error messages.
  // This relies on the fact that reinterpret_cast can add constness, but cannot remove it.
  return reinterpret_cast<D*>(reinterpret_cast<tdfnonstd::same_cv_t<char, D>*>(ptr) + byteOffset);
}

template <typename T, T* P>
struct FunctionWrapper {
  template <typename... Args>
  auto operator()(Args&&... args) const -> decltype(P(std::forward<Args>(args)...)) {
    return P(std::forward<Args>(args)...);
  }
};

/**
    Call a function when this goes out of scope. The template uses two
    parameters, the object, and a function that is to be called in the destructor.
    If release() is called, the object reference is set to null. If the object
    reference is null when the destructor is called, we do not call the
    function.
*/
template <typename T, void (*P)(T*)>
class AutoTCallVProc : public std::unique_ptr<T, FunctionWrapper<std::remove_pointer_t<decltype(P)>, P>> {
  using inherited = std::unique_ptr<T, FunctionWrapper<std::remove_pointer_t<decltype(P)>, P>>;

 public:
  using inherited::inherited;
  AutoTCallVProc(const AutoTCallVProc&) = delete;
  AutoTCallVProc(AutoTCallVProc&& that) : inherited(std::move(that)) {}

  operator T*() const { return this->get(); }
};
}  // namespace tdfcore
