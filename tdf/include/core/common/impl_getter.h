//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <any>
#include <memory>

namespace tdfcore {

class ImplGetter {
 public:
  template <typename T>
  T& GetImplRef() {
    return *(std::any_cast<T*>(Get()));
  }

  template <typename T>
  const T& GetImplConstRef() const {
    return *(std::any_cast<const T*>(GetConst()));
  }

 protected:
  // 返回实际后端的指针 const T* / T* ，注意这里仅用 const 重载是不够的，故区分命名
  virtual std::any GetConst() const = 0;
  virtual std::any Get() = 0;
};

}  // namespace tdfcore
