//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <unordered_map>
#include <string>
#include "core/common/macro.h"
#include "core/common/time.h"

namespace tdfcore {

struct GraphicMemoryObject {
  // 资源唯一标识
  std::string name;
  // 类型
  std::string type;
  // 分类
  std::string category;
  // size的单位
  std::string units;
  // 大小
  size_t size = 0;
};

using GraphicMemoryObjectMap = std::unordered_map<std::string, GraphicMemoryObject>;

struct GraphicMemoryObjects {
  // 时间戳
  RealTimePoint timestamp;
  // 一次采集产生的所有内存对象集合
  GraphicMemoryObjectMap objects;
};

}  // namespace tdfcore
