//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <functional>
#include <map>
#include <string>
#include "core/common/memory.h"
#include "core/common/property.h"

namespace tdfcore {
class Diagnostics;

#define HASH_FROM_POINTER(ptr) reinterpret_cast<int64_t>(ptr)
#define HASH_FROM_THIS HASH_FROM_POINTER(this)

// 此宏将尝试获取调用者的类名（以下为不启用RTTI时的实现）
#define CURRENT_CLASS_NAME GetBriefTypeNameFromFunctionName(__PRETTY_FUNCTION__, __FUNCTION__)

std::string GetTypeNameFromFunctionName(const std::string &pretty_func, const std::string &func);

std::string GetBriefTypeNameFromFunctionName(const std::string &pretty_func, const std::string &func);

/**
 * @brief 【抽象类】表示能够调试诊断的对象，可根据自身属性，给出必要的调试诊断信息（Diagnostics）
 */
class Diagnosticable {
 public:
  /**
   * @brief 生成当前节点的调试诊断信息
   */
  virtual std::shared_ptr<Diagnostics> ToDiagnostics() const = 0;

  virtual ~Diagnosticable() = default;
};

/**
 * @brief 调试诊断信息
 */
class Diagnostics : public Object {
 public:
  /**
   * @brief 静态构造方法
   */
  static std::shared_ptr<Diagnostics> Make(const std::string &name, int64_t hash = 0);

  ~Diagnostics() override = default;

  /**
   * @brief 获取名称
   */
  constexpr const std::string &Name() const { return name_; }

  /**
   * @brief 获取哈希值
   */
  constexpr int64_t Hash() const { return hash_; }

  /**
   * @brief 获取可诊断属性列表
   */
  const std::map<std::string, std::shared_ptr<Property>> &Properties();

  /**
   * @brief 添加一个可诊断属性
   */
  void AddProperty(const std::shared_ptr<Property> &prop);

  /**
   * @brief 序列化为字符串
   */
  std::string ToString() const override;

  /**
   * @brief 序列化为JSON字符串
   */
  std::string ToJSONString();

 protected:
  std::string name_;
  int64_t hash_;
  std::map<std::string, std::shared_ptr<Property>> properties_;

  explicit Diagnostics(const std::string &name, int64_t hash = 0);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
