//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include "core/common/object.h"

namespace tdfcore {

enum {
  T_MALLOC_ZERO_INITIALIZE = 1 << 0,
};

extern void* t_malloc_flags(size_t size, unsigned flags);

static inline void* t_calloc_canfail(size_t size) { return t_malloc_flags(size, T_MALLOC_ZERO_INITIALIZE); }

extern void t_free(void*);

class TData final : public Object {
 public:
  size_t size() const { return size_; }

  bool isEmpty() const { return 0 == size_; }

  const void* data() const { return ptr_; }

  const uint8_t* bytes() const { return reinterpret_cast<const uint8_t*>(ptr_); }

  void* writable_data() { return const_cast<void*>(ptr_); }

  size_t copyRange(size_t offset, size_t length, void* buffer) const;

  bool equals(const TData* other) const;

  typedef void (*ReleaseProc)(const void* ptr, void* context);

  static std::shared_ptr<TData> MakeWithCopy(const void* data, size_t length);

  static std::shared_ptr<TData> MakeUninitialized(size_t length);

  static std::shared_ptr<TData> MakeZeroInitialized(size_t length);

  static std::shared_ptr<TData> MakeWithProc(const void* ptr, size_t length, ReleaseProc proc, void* ctx);

  static std::shared_ptr<TData> MakeWithoutCopy(const void* data, size_t length) {
    return MakeWithProc(data, length, NoopReleaseProc, nullptr);
  }

  static std::shared_ptr<TData> MakeFromMalloc(const void* data, size_t length);

  static std::shared_ptr<TData> MakeFromFileName(const char path[]);

  static std::shared_ptr<TData> MakeFromFILE(FILE* f);

  static std::shared_ptr<TData> MakeEmpty();

  // private:
  ReleaseProc release_proc_;
  void* release_proc_context_;
  const void* ptr_;
  size_t size_;

  TData(const void* ptr, size_t size, ReleaseProc, void* context);
  explicit TData(size_t size);  // inplace new/delete
  ~TData() override;

  // Ensure the unsized delete is called.
  void operator delete(void* p);

  // shared internal factory
  static std::shared_ptr<TData> PrivateNewWithCopy(const void* src_or_null, size_t length);

  static void NoopReleaseProc(const void*, void*);
};

}  // namespace tdfcore
