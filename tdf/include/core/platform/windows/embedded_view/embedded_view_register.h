//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <map>
#include <unordered_map>
#include "core/common/object.h"

namespace tdfcore {
class EmbeddedHostView;
/**
 * @brief EmbeddedHostView注册器
 */
class EmbeddedViewRegister : public Object {
 public:
  /// EmbeddedHostView构造方法
  using EmbeddedHostViewCreator =
      std::function<std::shared_ptr<EmbeddedHostView>(const std::map<std::string, std::string>&)>;
  void RegisterEmbeddedViewCreator(const std::string& view_type, const EmbeddedHostViewCreator& creator);
  EmbeddedHostViewCreator GetEmbeddedViewCreator(const std::string& view_type) const;

 private:
  std::unordered_map<std::string, EmbeddedHostViewCreator> creator_map_;
};

}  // namespace tdfcore
