//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "core/common/data.h"
#include "core/common/m33.h"
#include "core/common/m44.h"
#include "core/common/point.h"
#include "core/common/size.h"
#include "core/common/time.h"
#include "core/common/object.h"

/**
 * @note 此文件提供的BinaryOutputArchive与BinaryInputArchive类仅在Core内部使用，请勿在外部直接使用它们
 * @brief 本文件中实现了以二进制方式对数据做序列化与反序列化的归档类
 */

#define TDF_BINARY_WRITE_VALUE_FUNC(Type) \
  inline void WriteField(Type &field) { stream_->write(reinterpret_cast<const char *>(&field), sizeof(Type)); }
#define TDF_BINARY_READ_VALUE_FUNC(Type) \
  inline void ReadField(Type &field) { stream_->read(reinterpret_cast<char *>(&field), sizeof(Type)); }

#define TDF_BINARY_WRITE_BUFFER_FUNC(Type, arg_name, ptr, length)                \
  inline void WriteField(Type &arg_name) {                                       \
    uint32_t __length = static_cast<uint32_t>(length);                           \
    stream_->write(reinterpret_cast<const char *>(&__length), sizeof(uint32_t)); \
    if (__length > 0) {                                                          \
      stream_->write(reinterpret_cast<const char *>(ptr), __length);             \
    }                                                                            \
  }

#define TDF_BINARY_WRITE_SIZE(var, size)      \
  uint32_t var = static_cast<uint32_t>(size); \
  stream_->write(reinterpret_cast<const char *>(&var), sizeof(var))
#define TDF_BINARY_READ_SIZE(var) \
  uint32_t var = 0;               \
  stream_->read(reinterpret_cast<char *>(&var), sizeof(var))

namespace tdfcore {

template <typename E, typename R>
struct SerializableEnum {
  E *value;
  explicit SerializableEnum(E *value) : value(value) {}
  R ToRawValue() { return static_cast<R>(*value); }
  void FromRawValue(R raw_value) { *value = static_cast<E>(raw_value); }
};

class BinaryOutputArchive : public Object {
 public:
  ~BinaryOutputArchive() override = default;

  explicit BinaryOutputArchive(std::ostream *stream) : Object(), stream_(stream) { assert(stream_); }

  template <class... Types>
  inline void Save(Types &&...args) {
    Write(std::forward<Types>(args)...);
  }

 protected:
  std::ostream *stream_;

  template <class T>
  inline void Write(T &&field) {
    WriteField(field);
  }

  template <class T, class... Other>
  inline void Write(T &&head, Other &&...tail) {
    Write(std::forward<T>(head));
    Write(std::forward<Other>(tail)...);
  }

  TDF_BINARY_WRITE_VALUE_FUNC(bool);
  TDF_BINARY_WRITE_VALUE_FUNC(int8_t);
  TDF_BINARY_WRITE_VALUE_FUNC(uint8_t);
  TDF_BINARY_WRITE_VALUE_FUNC(int16_t);
  TDF_BINARY_WRITE_VALUE_FUNC(uint16_t);
  TDF_BINARY_WRITE_VALUE_FUNC(int32_t);
  TDF_BINARY_WRITE_VALUE_FUNC(uint32_t);
  TDF_BINARY_WRITE_VALUE_FUNC(int64_t);
  TDF_BINARY_WRITE_VALUE_FUNC(uint64_t);
  TDF_BINARY_WRITE_VALUE_FUNC(float);
  TDF_BINARY_WRITE_VALUE_FUNC(double);
  TDF_BINARY_WRITE_VALUE_FUNC(TPoint);
  TDF_BINARY_WRITE_VALUE_FUNC(TSize);
  TDF_BINARY_WRITE_VALUE_FUNC(TM44);
  TDF_BINARY_WRITE_BUFFER_FUNC(std::shared_ptr<TData>, field, field->data(), field->size());

  // For std::string、std::u16string、std::u32string
  template <typename T>
  inline void WriteField(std::basic_string<T> &field) {
    TDF_BINARY_WRITE_SIZE(size, field.size());
    if (size > 0) {
      stream_->write(reinterpret_cast<const char *>(field.c_str()), size * sizeof(T));
    }
  }

  inline void WriteField(TDuration &field) {
    int64_t value = field.count();
    stream_->write(reinterpret_cast<const char *>(&value), sizeof(TDuration::rep));
  }

  template <typename E, typename R>
  inline void WriteField(SerializableEnum<E, R> &field) {
    auto raw_value = field.ToRawValue();
    WriteField(raw_value);
  }

  template <typename T>
  inline typename std::enable_if<T::IsSerializableType::value, void>::type WriteField(T &field) {
    auto data = field.ToData();
    WriteField(data);
  }

  template <typename T>
  inline void WriteField(std::shared_ptr<T> &field) {
    bool valid = (field != nullptr);
    WriteField(valid);
    if (valid) {
      WriteField(*field.get());
    }
  }

  template <typename T>
  inline void WriteField(std::vector<T> &field) {
    TDF_BINARY_WRITE_SIZE(size, field.size());
    for (auto &item : field) {
      WriteField(item);
    }
  }

  template <typename K, typename V>
  inline void WriteField(std::map<K, V> &field) {
    TDF_BINARY_WRITE_SIZE(size, field.size());
    for (auto &pair : field) {
      auto key = pair.first;
      WriteField(key);
      WriteField(pair.second);
    }
  }
};

class BinaryInputArchive : public Object {
 public:
  ~BinaryInputArchive() override = default;

  explicit BinaryInputArchive(std::istream *stream) : Object(), stream_(stream) { assert(stream_); }

  template <class... Types>
  inline void Load(Types &&...args) {
    Read(std::forward<Types>(args)...);
  }

 protected:
  std::istream *stream_;

  template <class T>
  inline void Read(T &&field) {
    ReadField(field);
  }

  template <class T, class... Other>
  inline void Read(T &&head, Other &&...tail) {
    Read(std::forward<T>(head));
    Read(std::forward<Other>(tail)...);
  }

  TDF_BINARY_READ_VALUE_FUNC(bool);
  TDF_BINARY_READ_VALUE_FUNC(int8_t);
  TDF_BINARY_READ_VALUE_FUNC(uint8_t);
  TDF_BINARY_READ_VALUE_FUNC(int16_t);
  TDF_BINARY_READ_VALUE_FUNC(uint16_t);
  TDF_BINARY_READ_VALUE_FUNC(int32_t);
  TDF_BINARY_READ_VALUE_FUNC(uint32_t);
  TDF_BINARY_READ_VALUE_FUNC(int64_t);
  TDF_BINARY_READ_VALUE_FUNC(uint64_t);
  TDF_BINARY_READ_VALUE_FUNC(float);
  TDF_BINARY_READ_VALUE_FUNC(double);
  TDF_BINARY_READ_VALUE_FUNC(TM44);
  TDF_BINARY_READ_VALUE_FUNC(TPoint);
  TDF_BINARY_READ_VALUE_FUNC(TSize);

  // For std::string、std::u16string、std::u32string
  template <typename T>
  inline void ReadField(std::basic_string<T> &field) {
    TDF_BINARY_READ_SIZE(size);
    field.clear();
    for (; size > 0; --size) {
      T c;
      stream_->read(reinterpret_cast<char *>(&c), sizeof(T));
      field.push_back(c);
    }
  }

  inline void ReadField(std::shared_ptr<TData> &field) {
    TDF_BINARY_READ_SIZE(length);
    if (length > 0) {
      field = TData::MakeUninitialized(length);
      stream_->read(reinterpret_cast<char *>(field->writable_data()), length);
    } else {
      field = TData::MakeEmpty();
    }
  }

  inline void ReadField(TDuration &field) {
    int64_t value = 0;
    stream_->read(reinterpret_cast<char *>(&value), sizeof(TDuration::rep));
    field = TDuration(value);
  }

  template <typename E, typename R>
  inline void ReadField(SerializableEnum<E, R> &field) {
    R raw_value;
    ReadField(raw_value);
    field.FromRawValue(raw_value);
  }

  template <typename T>
  inline typename std::enable_if<T::IsSerializableType::value, void>::type ReadField(T &field) {
    std::shared_ptr<TData> data;
    ReadField(data);
    field.FromData(data);
  }

  template <typename T>
  inline void ReadField(std::shared_ptr<T> &field) {
    bool valid;
    ReadField(valid);
    if (valid) {
      if (!field) {
        field = std::shared_ptr<T>(new T());
      }
      ReadField(*field.get());
    } else {
      field = nullptr;
    }
  }

  template <typename T>
  inline void ReadField(std::vector<T> &field) {
    TDF_BINARY_READ_SIZE(size);
    field.clear();
    for (size_t i = 0; i < size; ++i) {
      T item;
      ReadField(item);
      field.push_back(item);
    }
  }

  template <typename K, typename V>
  inline void ReadField(std::map<K, V> &field) {
    TDF_BINARY_READ_SIZE(size);
    field.clear();
    for (size_t i = 0; i < size; ++i) {
      K key;
      ReadField(key);
      V value;
      ReadField(value);
      field[key] = value;
    }
  }
};

}  // namespace tdfcore
