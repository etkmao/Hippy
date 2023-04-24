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

#include "modules/network/network.h"

#include <memory>

#include "footstone/deserializer.h"
#include "footstone/logging.h"

namespace hippy {
inline namespace framework {
inline namespace windows {
inline namespace module {

constexpr char kHttpUrl[] = "url";

Network::Network() : network_adaptor_(std::make_shared<adaptor::HttpNetwork>()) {}

Network::Network(std::shared_ptr<hippy::adaptor::Network> network_adaptor)
    : network_adaptor_(std::move(network_adaptor)) {}

bool Network::Initial() {
  if (network_adaptor_) return network_adaptor_->Initial();
  return false;
}

bool Network::ParserRequestUrl(const footstone::value::HippyValue& value, std::string& url) {
  footstone::value::HippyValue::HippyValueArrayType parameters;
  bool ret = value.ToArray(parameters);
  if (!ret) return false;
  if (parameters.size() != 1) return false;

  if (parameters[0].IsObject()) {
    footstone::value::HippyValue::HippyValueObjectType objects;
    ret = parameters[0].ToObject(objects);
    if (!ret) return false;
    for (const auto& kv : objects) {
      if (kv.first == kHttpUrl) {
        ret = kv.second.ToString(url);
        return ret;
      }
    }
  } else if (parameters[0].IsString()) {
    return parameters[0].ToString(url);
  }

  return false;
}

void Network::Fetch(const std::shared_ptr<Context>& context, const footstone::value::HippyValue& request,
                    uint32_t runtime_id, std::function<void(footstone::value::HippyValue)> callback) {
  std::string url;
  auto ret = ParserRequestUrl(request, url);
  FOOTSTONE_DCHECK(ret);
  if (ret) {
    if (network_adaptor_ == nullptr) return;
    auto new_callback = [callback](std::string content) {
      footstone::Deserializer deserializer(reinterpret_cast<const uint8_t*>(&content[0]), content.size());
      deserializer.ReadHeader();
      footstone::HippyValue params;
      deserializer.ReadValue(params);
      callback(params);
    };
    network_adaptor_->Fetch(context, url, request, new_callback);
  }
}

void Network::GetCookie(const footstone::value::HippyValue& request, uint32_t runtime_id,
                        std::function<void(footstone::value::HippyValue)> callback) {
  std::string url;
  auto ret = ParserRequestUrl(request, url);
  FOOTSTONE_DCHECK(ret);
  if (ret) {
    if (network_adaptor_ == nullptr) return;
    network_adaptor_->GetCookie(url, callback);
  }
}

void Network::SetCookie(const footstone::value::HippyValue& request) {
  if (network_adaptor_ == nullptr) return;
  network_adaptor_->SetCookie(request);
}

}  // namespace module
}  // namespace windows
}  // namespace framework
}  // namespace hippy
