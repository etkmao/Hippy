//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace tdfcore {
template <typename T>
class TSpan {
 public:
  constexpr TSpan() : ptr_{nullptr}, size_{0} {}
  constexpr TSpan(T* ptr, size_t size) : ptr_{ptr}, size_{size} { assert(size < kMaxSize); }
  template <typename U, typename = typename std::enable_if<std::is_same<const U, T>::value>::type>
  constexpr TSpan(const TSpan<U>& that) : ptr_(that.data()), size_{that.size()} {}
  constexpr TSpan(const TSpan& o) = default;

  constexpr TSpan& operator=(const TSpan& that) = default;

  constexpr T& operator[](size_t i) const {
    assert(i < this->size());
    return ptr_[i];
  }
  constexpr T& front() const { return ptr_[0]; }
  constexpr T& back() const { return ptr_[size_ - 1]; }
  constexpr T* begin() const { return ptr_; }
  constexpr T* end() const { return ptr_ + size_; }
  constexpr auto rbegin() const { return std::make_reverse_iterator(this->end()); }
  constexpr auto rend() const { return std::make_reverse_iterator(this->begin()); }
  constexpr T* data() const { return this->begin(); }
  constexpr size_t size() const { return size_; }
  constexpr bool empty() const { return size_ == 0; }
  constexpr size_t size_bytes() const { return size_ * sizeof(T); }
  constexpr TSpan<T> first(size_t prefixLen) const {
    assert(prefixLen <= this->size());
    return TSpan{ptr_, prefixLen};
  }
  constexpr TSpan<T> last(size_t postfixLen) const {
    assert(postfixLen <= this->size());
    return TSpan{ptr_ + (this->size() - postfixLen), postfixLen};
  }
  constexpr TSpan<T> subspan(size_t offset) const { return this->subspan(offset, this->size() - offset); }
  constexpr TSpan<T> subspan(size_t offset, size_t count) const {
    assert(offset <= this->size());
    assert(count <= this->size() - offset);
    return TSpan{ptr_ + offset, count};
  }

 private:
  static constexpr size_t kMaxSize = std::numeric_limits<size_t>::max() / sizeof(T);
  T* ptr_;
  size_t size_;
};

template <typename T, typename S>
inline constexpr TSpan<T> MakeSpan(T* p, S s) {
  return TSpan<T>{p, static_cast<size_t>(s)};
}

template <size_t N, typename T>
inline constexpr TSpan<T> MakeSpan(T (&a)[N]) {
  return TSpan<T>{a, N};
}

template <typename Container>
inline auto MakeSpan(Container& c) -> TSpan<typename std::remove_reference<decltype(*(c.data()))>::type> {
  return {c.data(), c.size()};
}

}  // namespace tdfcore