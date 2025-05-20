/*
 * Tencent is pleased to support the open source community by making
 * Hippy available.
 *
 * Copyright (C) 2022 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "connector/worker_napi.h"
#include "connector/worker_module_manager.h"
#include <set>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <node_api.h>
#include "oh_napi/data_holder.h"
#include "oh_napi/oh_napi_register.h"
#include "oh_napi/ark_ts.h"
#include "footstone/check.h"
#include "footstone/logging.h"

namespace hippy {
inline namespace framework {
inline namespace connector {
inline namespace worker {

static void CallTs(napi_env env, napi_value ts_cb, void *context, void *data) {
  if (env == nullptr || data == nullptr) {
    return;
  }
  FnContextData *contextData = static_cast<FnContextData*>(data);
  
  ArkTS arkTs(env);
  std::vector<napi_value> args = {
    arkTs.CreateStringUtf16(contextData->module_str_),
    arkTs.CreateStringUtf16(contextData->func_str_),
    arkTs.CreateStringUtf16(contextData->cb_id_str_),
    arkTs.CreateExternalArrayBuffer(contextData->buffer_pair_.first, contextData->buffer_pair_.second)
  };
  arkTs.Call(ts_cb, args, nullptr);
}

static napi_value RegisterWModules(napi_env env, napi_callback_info info) {
  ArkTS arkTs(env);
  auto args = arkTs.GetCallbackArgs(info);
  auto ts_cb = args[0];
  auto ts_array = args[1];
  
  // 使用回调ts的函数创建一个线程安全函数
  napi_threadsafe_function func = nullptr;
  napi_value resource_name = arkTs.CreateString("Thread-safe Function callNatives");
  auto status = napi_create_threadsafe_function(env, ts_cb, nullptr, resource_name, 0, 1, nullptr, nullptr,
    nullptr, CallTs, &func);
  if (status != napi_ok) {
    FOOTSTONE_LOG(ERROR) << "ArkTS: Failed to create thread safe func, status: " << status;
    return arkTs.GetUndefined();
  }
  // TODO(hot): release func
  
  // 模块名数组
  std::set<std::string> module_names;
  if (arkTs.IsArray(ts_array)) {
    auto length = arkTs.GetArrayLength(ts_array);
    if (length > 0) {
      for (uint32_t i = 0; i < length; i ++) {
        auto ts_name = arkTs.GetArrayElement(ts_array, i);
        auto name = arkTs.GetString(ts_name);
        if (name.length() > 0) {
          module_names.insert(name);
        }
      }
    }
  }
  
  if (module_names.size() > 0) {
    WorkerModuleManager::GetInstance()->SetWModules(env, func, module_names);
  }
  
  return arkTs.GetUndefined();
}

REGISTER_OH_NAPI("Worker", "Worker_RegisterWModules", RegisterWModules)

}
}
}
}
