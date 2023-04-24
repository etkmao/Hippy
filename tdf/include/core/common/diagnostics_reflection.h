//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/reflection.h"
#include "core/common/diagnosticable.h"

namespace tdfcore {

/**
 * @brief 【快捷方法】为一个对象生成诊断信息，它所携带的Preperty字段由类型T中的可反射成员变量组成
 * @param object 对象指针
 */
template<class T>
std::shared_ptr<Diagnostics> MakeDiagnosticsByReflection(const T *object) {
  auto diagnostics = Diagnostics::Make(object->GetType().GetName(), HASH_FROM_POINTER(object));
  for (auto const &property : object->GetReflection()->GetProperties(object)) {
    diagnostics->AddProperty(property);
  }
  return diagnostics;
}

}  // namespace tdfcore
