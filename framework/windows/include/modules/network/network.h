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

#include <memory>

#include "adaptor/network/http_network.h"
#include "context.h"
#include "footstone/hippy_value.h"
#include "vfs/vfs_manager.h"

namespace hippy {
inline namespace framework {
inline namespace windows {
inline namespace module {

class Network {
 public:
  Network();
  Network(std::shared_ptr<hippy::adaptor::Network> network_adaptor);
  ~Network() = default;

  bool Initial();
  void Fetch(const std::shared_ptr<Context>& context, const footstone::value::HippyValue& request, uint32_t runtime_id,
             std::function<void(footstone::value::HippyValue)> callback);
  void GetCookie(const footstone::value::HippyValue& request, uint32_t runtime_id,
                 std::function<void(footstone::value::HippyValue)> callback);
  void SetCookie(const footstone::value::HippyValue& request);

 private:
  bool ParserRequestUrl(const footstone::value::HippyValue& value, std::string& url);

 private:
  std::shared_ptr<hippy::adaptor::Network> network_adaptor_;
};

}  // namespace module
}  // namespace windows
}  // namespace framework
}  // namespace hippy