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

#pragma once

#include "adaptor/netinfo/net_info.h"

#include <functional>
#include <string>

#include <WinSock2.h>
#include <netioapi.h>

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

class AdapterAddressesNetInfo : public NetInfo {
 public:
  AdapterAddressesNetInfo() = default;
  virtual ~AdapterAddressesNetInfo() override = default;
  AdapterAddressesNetInfo(const AdapterAddressesNetInfo&) = delete;
  AdapterAddressesNetInfo(AdapterAddressesNetInfo&&) = delete;
  AdapterAddressesNetInfo& operator=(const AdapterAddressesNetInfo&) = delete;
  AdapterAddressesNetInfo& operator=(AdapterAddressesNetInfo&&) = delete;

  virtual bool Initial();
  virtual void GetCurrentConnectivity() override;
  virtual void SetCurrentConnectivityCallback(std::function<void(std::string network_info)> callback) override;

 private:
  void GetNetInfo();
  void GetAdaptersNetInfo();

  std::string current_network_info_;
  std::function<void(std::string network_info)> connectivity_callback_;
};

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
