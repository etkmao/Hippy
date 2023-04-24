//
// Created by ashezhang on 2021/12/1.
// Copyright (c) 2021 Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>

namespace tdfcore {

#define TDF_DEFER(func) auto TDF_COMBINE(defer_, __COUNTER__) = Defer(func)

class Defer {
 public:
  explicit Defer(std::function<void()> defer) : func_(defer) {}
  Defer(const Defer&) = delete;
  Defer& operator=(const Defer&) = delete;

  Defer(Defer&& defer) noexcept { defer.func_.swap(func_); }
  Defer& operator=(Defer&& defer) noexcept {
    defer.func_.swap(this->func_);
    return *this;
  }

  virtual ~Defer() {
    if (func_) {
      func_();
    }
  }

 private:
  std::function<void()> func_{};
};

}  // namespace tdfcore
