//
// Copyright (c) 2021 com.tencent. All rights reserved.
//
#pragma once
#include "core/common/object.h"

namespace tdfcore {

class ByteStream : public Object {
 public:
  /**
   * 用于析构后data的释放处理
   */
  using RunFinalizer = std::function<void(const void* data, void* context)>;

  ByteStream(const void* data, size_t size, RunFinalizer run_finalizer = nullptr, void* context = nullptr);
  ByteStream(const ByteStream&) = delete;
  ByteStream& operator=(const ByteStream&) = delete;

  ~ByteStream() override;

  const void* GetData() const;

  size_t GetSize() const;

 private:
  const void* data_;
  size_t data_size_;
  RunFinalizer run_finalizer_;
  void* context_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
