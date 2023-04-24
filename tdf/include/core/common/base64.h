//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <string>
#include "core/common/macro.h"

namespace tdfcore {

class Base64 {
 public:
  static std::string Encode(const uint8_t *bin, size_t len);
  static std::string Decode(const std::string& input);
};
}  // namespace tdfcore