//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <cassert>
#include <cstring>
#include "core/common/object.h"

namespace tdfcore {

/**
 * @brief 字节流数据读写类
 */
class DataStream : public Object {
 public:
  /**
   * @brief 构造函数
   * @param capacity 初始最大容量；数据缓冲区由DataStream内部提供，可无限扩容
   */
  explicit DataStream(size_t capacity = kDefaultCapacity) : Object(), capacity_(capacity) {
    buffer_ = new uint8_t[capacity_];
    assert(buffer_);
  }

  ~DataStream() override { delete[] buffer_; }

  /***
   * @brief 读取数据到目标Buffer
   * @param dst 目标Buffer地址
   * @param size 数据大小
   * @return
   */
  size_t Read(void* dst, size_t size) {
    // 检查剩余的大小是否足够目标数据大小
    CheckReadableSize(size);
    memcpy(dst, buffer_ + out_pos_, size);
    out_pos_ += size;
    return size;
  }

  /***
   * @brief 读取基础类型数据
   * @param i
   */
  template <typename T>
  DataStream& operator>>(T& output) {
    CheckReadableSize(sizeof(T));
    memcpy(&output, buffer_ + out_pos_, sizeof(T));
    out_pos_ += sizeof(T);
    return *this;
  }

  /***
   * @brief 写入src数据
   * @param src 要写入的愿数据
   * @param size 写入长度
   * @return
   */
  size_t Write(const void* src, size_t size) {
    CheckWritableSize(size);
    memcpy(buffer_ + in_pos_, src, size);
    in_pos_ += size;
    return size;
  }

  /***
   * @brief 写入数据到DataStream
   * @param input
   */
  template <typename T>
  DataStream& operator<<(T input) {
    CheckWritableSize(sizeof(T));
    memcpy(buffer_ + in_pos_, &input, sizeof(T));
    in_pos_ += sizeof(T);
    return *this;
  }

  /***
   * @brief 将写入位置设置到pos位置
   * @param pos
   */
  void SeekIn(size_t pos) {
    assert(pos < capacity_);
    in_pos_ = pos;
  }

  /***
   * @brief 将读取位置设置到pos位置
   * @param pos
   */
  void SeekOut(size_t pos) {
    assert(pos < capacity_);
    out_pos_ = pos;
  }

  uint8_t* GetData() const { return buffer_; }

  constexpr size_t GetCapacity() const { return capacity_; }

  constexpr size_t GetInPos() const { return in_pos_; }

  constexpr size_t GetOutPos() const { return out_pos_; }

 private:
  void CheckWritableSize(size_t need_size) {
    auto remain_size = capacity_ - in_pos_;
    if (remain_size < need_size) {
      // 自增长扩容
      size_t capacity = capacity_ * 2;
      uint8_t* buffer = new uint8_t[capacity];
      assert(buffer);
      memcpy(buffer, buffer_, capacity_);
      delete[] buffer_;
      buffer_ = buffer;
      capacity_ = capacity;
    }
  }

  void CheckReadableSize(size_t need_size) const { assert((capacity_ - out_pos_) >= need_size); }

 private:
  constexpr static const size_t kDefaultCapacity = 1024;
  uint8_t* buffer_;
  size_t capacity_ = 0;
  size_t in_pos_ = 0;
  size_t out_pos_ = 0;
};

}  // namespace tdfcore
