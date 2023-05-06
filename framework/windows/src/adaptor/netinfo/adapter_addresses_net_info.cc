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

#include "adaptor/netinfo/adapter_addresses_net_info.h"

#include <WinSock2.h>
#include <Wininet.h>
#include <iphlpapi.h>

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

constexpr char kConnectionTypeNone[] = "NONE";
constexpr char kConnectionTypeWifi[] = "WIFI";
constexpr char kConnectionTypeCell[] = "CELL";
constexpr char kConnectionTypeEthernet[] = "ETHERNET";
constexpr char kConnectionTypeUnknow[] = "UNKNOWN";

static void CALLBACK NetworkChangeCallback(PVOID caller_context, PMIB_IPINTERFACE_ROW row,
                                           MIB_NOTIFICATION_TYPE notification_type) {
  auto self = reinterpret_cast<AdapterAddressesNetInfo*>(caller_context);
  self->GetCurrentConnectivity();
}

bool AdapterAddressesNetInfo::Initial() {
  HANDLE hNotify;
  return NotifyIpInterfaceChange(AF_UNSPEC, &NetworkChangeCallback, reinterpret_cast<PVOID>(this), false, &hNotify) !=
         NO_ERROR;
}

// TODO(charleeshen): 获取网络数据不一定准，需要在确认一下
void AdapterAddressesNetInfo::GetNetInfo() {
  // https://learn.microsoft.com/en-us/windows/win32/api/wininet/nf-wininet-internetgetconnectedstate
  DWORD flags = 0;
  if (!InternetGetConnectedState(&flags, 0)) {
    current_network_info_ = kConnectionTypeNone;
    return;
  }

  // INTERNET_CONNECTION_CONFIGURED：配置好了网络，但是没有连接网络
  // INTERNET_CONNECTION_OFFLINE：处于离线状态，无法连接到网络
  if ((flags & INTERNET_CONNECTION_CONFIGURED) || (flags & INTERNET_CONNECTION_OFFLINE)) {
    current_network_info_ = kConnectionTypeNone;
    // INTERNET_CONNECTION_LAN：当前连接的是局域网。
    // INTERNET_CONNECTION_MODEM：当前连接的是调制解调器。
    // INTERNET_CONNECTION_PROXY：当前连接的是代理服务器。
    // INTERNET_RAS_INSTALLED: 系统安装了远程访问服务。
  } else if ((flags & INTERNET_CONNECTION_LAN) || (flags & INTERNET_CONNECTION_MODEM) ||
             (flags & INTERNET_CONNECTION_PROXY) || (flags & INTERNET_RAS_INSTALLED)) {
    GetAdaptersNetInfo();
  } else {
    current_network_info_ = kConnectionTypeUnknow;
  }
}

void AdapterAddressesNetInfo::GetAdaptersNetInfo() {
  PIP_ADAPTER_ADDRESSES adapter_addresses = NULL;
  ULONG size = 0;

  // 获取适配器信息
  auto ret = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_GATEWAYS, NULL, adapter_addresses, &size);
  if (ret == ERROR_BUFFER_OVERFLOW) {
    adapter_addresses = (PIP_ADAPTER_ADDRESSES)malloc(size);
    ret = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_GATEWAYS, NULL, adapter_addresses, &size);
  }
  if (ret == NO_ERROR) {
    PIP_ADAPTER_ADDRESSES adapter = adapter_addresses;
    while (adapter) {
      // 判断适配器类型
      if (adapter->IfType == IF_TYPE_IEEE80211) {  // WIFI
        current_network_info_ = kConnectionTypeWifi;
        free(adapter_addresses);
        return;
      } else if (adapter->IfType == IF_TYPE_ETHERNET_CSMACD) {  // 有线网络
        current_network_info_ = kConnectionTypeEthernet;
        free(adapter_addresses);
        return;
      } else if (adapter->IfType == IF_TYPE_IEEE1394) {  // 无线网络
        current_network_info_ = kConnectionTypeCell;
        free(adapter_addresses);
        return;
      }
      adapter = adapter->Next;
    }
  }
  current_network_info_ = kConnectionTypeUnknow;
  free(adapter_addresses);
}

void AdapterAddressesNetInfo::GetCurrentConnectivity() {
  if (!connectivity_callback_) return;
  GetNetInfo();
  connectivity_callback_(current_network_info_);
}

void AdapterAddressesNetInfo::SetCurrentConnectivityCallback(std::function<void(std::string network_info)> callback) {
  connectivity_callback_ = callback;
}

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
