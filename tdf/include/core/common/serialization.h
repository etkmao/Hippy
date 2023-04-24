//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/binary_archive.h"
#include "core/common/macro.h"

namespace tdfcore {

/**
 * @brief 可进行序列化操作的对象
 */
class Serializable {
 public:
  virtual ~Serializable() = default;

  /**
   * @brief 序列化
   * @return 序列化后的二进制数据
   */
  virtual std::shared_ptr<TData> Serialize() = 0;
};

}  // namespace tdfcore

/**
 * @brief 在类or结构体的声明中使用宏TDF_SERIALIZATION，自动实现序列化与反序列方法（目前仅支持二进制序列化）
 * @detail
 * 样例：
 *   namespace tdf {
 *     struct Foo {
 *       int64_t i;
 *       float f;
 *       std::string s;
 *       NestedType n;
 *       std::vector<float> vf;
 *       std::vector<NestedType> vn;
 *       ET e;
 *
 *       TDF_SERIALIZATION(i, f, s, n, vf, vn, TDF_SERIALIZABLE_ENUM(e, ET, uint_8));
 *     };
 *
 *     enum class ET : uint_8 {
 *       kA,
 *       kB,
 *     }
 *
 *     struct NestedType {
 *       int64_t a;
 *       double b;
 *       TDF_SERIALIZATION(a, b);
 *     };
 *   }
 *
 * 使用方式：
 *   在类or结构体的声明中，加入宏TDF_SERIALIZATION，在其()内依次列举需要参与序列化的成员变量；
 *
 * 已经支持的类型：
 *   （1）常见的数值类型，包括整型、浮点型、字符串；
 *   （2）一些常用std类型，包括vector、map、shared_ptr；
 *   （3）enum类型，但需要用TDF_SERIALIZABLE_ENUM宏对成员变量进行包裹；
 *   （4）已通过TDF_SERIALIZATION实现序列化能力的类型；例如样例中的NestedType；
 *   （5）其它类型待补充支持；
 */
#define TDF_SERIALIZATION(...)                                                                          \
  using IsSerializableType = std::integral_constant<bool, true>;                                        \
  std::shared_ptr<::tdfcore::TData> ToData() {                                                          \
    std::ostringstream stream;                                                                          \
    ::tdfcore::BinaryOutputArchive archive(&stream);                                                    \
    archive.Save(__VA_ARGS__);                                                                          \
    return ::tdfcore::TData::MakeWithCopy(stream.str().c_str(), stream.str().length());                 \
  }                                                                                                     \
  void FromData(const std::shared_ptr<::tdfcore::TData> &data) {                                        \
    assert(data && !data->isEmpty());                                                                   \
    std::istringstream stream(std::string(reinterpret_cast<const char *>(data->data()), data->size())); \
    ::tdfcore::BinaryInputArchive archive(&stream);                                                     \
    archive.Load(__VA_ARGS__);                                                                          \
  }

// 枚举类型的成员变量，需用该宏进行包裹
#define TDF_SERIALIZABLE_ENUM(value, enum_type, raw_type) ::tdfcore::SerializableEnum<enum_type, raw_type>(&value)
