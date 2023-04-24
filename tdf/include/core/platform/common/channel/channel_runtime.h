//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <string>
#include <unordered_map>

#include "core/common/object.h"
#include "core/platform/common/channel/host_object.h"
#include "core/platform/common/channel/native_value_wrapper.h"

namespace tdfcore {

class ChannelRuntime : public Object {
 public:
  ChannelRuntime() = default;

  virtual std::shared_ptr<HostObject> GetHostObject(const std::string &name);

  ~ChannelRuntime() override;

  void RegisterHostObject(const std::string &name, std::shared_ptr<HostObject> host_object);

 protected:
  std::unordered_map<std::string, std::shared_ptr<HostObject>> host_obj_cache_;
};

}  // namespace tdfcore
