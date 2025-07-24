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
 *     http://www.apache.org/licenses/LICENSE2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "driver/vm/jsh/inspector/jsh_inspector_client_impl.h"
#include <unistd.h>
#include "driver/napi/jsh/jsh_ctx.h"
#include "driver/scope.h"
#include "devutils/http_client.h"
#include "devutils/ws_client.h"
#include "driver/scope.h"
#include "oh_napi/oh_napi_task_runner.h"


namespace hippy {
inline namespace driver {
inline namespace runtime {
inline namespace inspector {

using WsClient = hippy::devtools::WsClient;

std::shared_ptr<WsClient> wsc_;

void JSHInspectorClientImpl::Start() {
  auto http = std::make_shared<devtools::HttpClient>();
  std::string body;
  http->Connect(body);
  
  FOOTSTONE_LOG(INFO) << "xxx hippy, body: " << body;

  // [ {\n  \"description\": \"node.js instance\",\n  \"devtoolsFrontendUrl\": \"devtools://devtools/bundled/js_app.html?v8only=true&ws=localhost/18fb577b-9ea0-4ba9-b7ff-11593f9f417c\",\n  \"devtoolsFrontendUrlCompat\": \"devtools://devtools/bundled/inspector.html?v8only=true&ws=localhost/18fb577b-9ea0-4ba9-b7ff-11593f9f417c\",\n  \"faviconUrl\": \"https://nodejs.org/static/images/favicons/favicon.ico\",\n  \"id\": \"18fb577b-9ea0-4ba9-b7ff-11593f9f417c\",\n  \"title\": \"Node.js[26373]\",\n  \"type\": \"node\",\n  \"url\": \"file://\",\n  \"webSocketDebuggerUrl\": \"ws://localhost/18fb577b-9ea0-4ba9-b7ff-11593f9f417c\"\n} ]\n\n
  
  //[
  //  {
  //    "description": "node.js instance",
  //    "devtoolsFrontendUrl": "devtools://devtools/bundled/js_app.html?v8only=true&ws=localhost:9229/cb3fcc9f-fc0f-403e-8aec-f60e859435a5",
  //    "devtoolsFrontendUrlCompat": "devtools://devtools/bundled/inspector.html?v8only=true&ws=localhost:9229/cb3fcc9f-fc0f-403e-8aec-f60e859435a5",
  //    "faviconUrl": "https://nodejs.org/static/images/favicons/favicon.ico",
  //    "id": "cb3fcc9f-fc0f-403e-8aec-f60e859435a5",
  //    "title": "Node.js[4035]",
  //    "type": "node",
  //    "url": "file://",
  //    "webSocketDebuggerUrl": "ws://localhost:9229/cb3fcc9f-fc0f-403e-8aec-f60e859435a5"
  //  }
  //]
  
  std::string devUrlKey = "\"devtoolsFrontendUrl\"";
  auto pos = body.find(devUrlKey);
  if (pos != std::string::npos) {
    auto urlBegin = body.find("\"", pos + devUrlKey.length() + 1);
    urlBegin += 1;
    auto urlEnd = body.find("\"", urlBegin);
    auto devUrl = body.substr(urlBegin, urlEnd - urlBegin);
    
    FOOTSTONE_LOG(INFO) << "xxx hippy, devUrl: " << devUrl;
    
    std::string wsUrlKey = "ws=localhost/";
    auto wsBegin = devUrl.find(wsUrlKey);
    wsBegin += wsUrlKey.length();
    std::string wsUrl = devUrl.substr(wsBegin);
    
    wsUrl = "ws://127.0.0.1:9225/" + wsUrl;
    
    FOOTSTONE_LOG(INFO) << "xxx hippy, wsUrl: " << wsUrl;
    
    wsc_ = std::make_shared<WsClient>();
    wsc_->Connect(wsUrl, [WEAK_THIS](const std::string& msg) {
      DEFINE_AND_CHECK_SELF(JSHInspectorClientImpl)
      self->HandleReceiveData(msg);
    });
  }
}

void JSHInspectorClientImpl::HandleReceiveData(const std::string& msg) {
  FOOTSTONE_LOG(INFO) << "xxx hippy, ws recv data: " << msg;

  if (devtools_data_source_) {
    devtools_data_source_->GetNotificationCenter()->vm_response_notification->ResponseToFrontend(msg);
  }
}

void JSHInspectorClientImpl::CreateInspector(const std::shared_ptr<Scope>& scope) {
  OhNapiTaskRunner *taskRunner = OhNapiTaskRunner::Instance(OhNapiTaskRunner::main_env_);
  taskRunner->RunAsyncTask([this]() { // TODO(hot): this
    sleep(2); // TODO(hot):
    this->Start();
  });
  
//  auto thread = std::thread([this]() -> void { // TODO(hot): this
//    sleep(2); // TODO(hot):
//    this->inspector_client_ = std::make_shared<JSHInspectorClient>();
//    this->inspector_client_->Start();
//    FOOTSTONE_LOG(INFO) << "xxx hippy, thread end";
//  });
  
  auto ctx = std::static_pointer_cast<hippy::napi::JSHCtx>(scope->GetContext());
  auto env = ctx->env_;
  OH_JSVM_OpenInspector(env, "localhost", 9225);
  OH_JSVM_WaitForDebugger(env, true);
}

void JSHInspectorClientImpl::DestroyInspector(bool is_reload) {
//  OH_JSVM_CloseInspector(env_);
}

void JSHInspectorClientImpl::SendMessageToJSH(const std::string&& params) {
  wsc_->Send(params);
}

} // namespace inspector
} // namespace runtime
} // namespace driver
} // namespace hippy
