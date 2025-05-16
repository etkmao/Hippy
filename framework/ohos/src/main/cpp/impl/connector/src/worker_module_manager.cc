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

#include "connector/worker_module_manager.h"
#include <js_native_api.h>
#include <js_native_api_types.h>
#include "connector/turbo.h"
#include "driver/scope.h"
#include "oh_napi/data_holder.h"
#include "oh_napi/oh_napi_invocation.h"
#include "oh_napi/oh_napi_register.h"
#include "oh_napi/ark_ts.h"
#include "oh_napi/oh_napi_object.h"
#include "oh_napi/oh_napi_register.h"

#include "oh_napi/oh_napi_task_runner.h"


#include <cstdint>

#include "footstone/logging.h"
#include "footstone/string_view.h"
#include "footstone/string_view_utils.h"


using namespace hippy::napi;
using string_view = footstone::string_view;
using StringViewUtils = footstone::StringViewUtils;


namespace hippy {
inline namespace framework {
inline namespace worker {

std::shared_ptr<WorkerModuleManager> WorkerModuleManager::GetInstance() {
  static std::shared_ptr<WorkerModuleManager> instance = nullptr;
  static std::once_flag flag;
  std::call_once(flag, [] {
    instance = std::make_shared<WorkerModuleManager>();
  });
  return instance;
}

}
}
}
