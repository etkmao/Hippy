//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <any>
#include <cassert>
#include <string>
#include <unordered_map>
#include "core/common/macro.h"
#include "core/common/memory.h"

namespace tdfcore {

class Object {
 public:
  virtual ~Object();

  virtual std::string ToString() const { return ""; }

  void SetId(uint64_t id) { id_ = id; }

  constexpr uint64_t GetId() const { return id_; }

  void SetDebugLabel(const std::string &label) { debug_label_ = label; }

  constexpr const std::string &GetDebugLabel() const { return debug_label_; }

  virtual const char *GetDebugCategory() const { return nullptr; }

  virtual void Init() {}

  void SetExfield(const std::string &key, const std::any &value) {
    exfield_[key] = value;
  }

  const std::any &GetExfield(const std::string &key) {
    return exfield_[key];
  }

 protected:
  /**
   * 对象的id，有多种用途。
   * 0是非法id。
   */
  uint64_t id_ = 0;
  std::string debug_label_;
  std::unordered_map<std::string, std::any> exfield_;

 private:
  bool is_traced_ = false;
  void Trace(const char *label, int64_t bytes, const char *file, int64_t line);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
