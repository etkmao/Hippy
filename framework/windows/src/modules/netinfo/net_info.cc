/*
 *
 * Tencent is pleased to support the open source community by making
 * Hippy available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "modules/netinfo/net_info.h"

#include <memory>

#include "adaptor/netinfo/adapter_addresses_net_info.h"

namespace hippy {
inline namespace framework {
inline namespace windows {
inline namespace module {

NetInfo::NetInfo() : net_info_adaptor_(std::make_shared<adaptor::AdapterAddressesNetInfo>()){};

NetInfo::NetInfo(std::shared_ptr<hippy::adaptor::NetInfo> net_info_adaptor)
    : net_info_adaptor_(std::move(net_info_adaptor)){};

bool NetInfo::Initial() {
  if (net_info_adaptor_) return net_info_adaptor_->Initial();
  return false;
}

void NetInfo::GetCurrentConnectivity(std::function<void(footstone::value::HippyValue params)> callback) {
  auto new_callback = [callback](std::string net_info) {
    footstone::HippyValue::HippyValueObjectType object;
    object.insert({"network_info", footstone::HippyValue(net_info)});
    callback(footstone::HippyValue(object));
  };
  net_info_adaptor_->SetCurrentConnectivityCallback(new_callback);
  net_info_adaptor_->GetCurrentConnectivity();
}

}  // namespace module
}  // namespace windows
}  // namespace framework
}  // namespace hippy
