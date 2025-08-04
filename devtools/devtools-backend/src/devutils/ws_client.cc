/*
 * Tencent is pleased to support the open source community by making
 * Hippy available.
 *
 * Copyright (C) 2017-2019 THL A29 Limited, a Tencent company.
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

#include "devutils/ws_client.h"
#include <asio/ip/tcp.hpp>
#include "nlohmann/json.hpp"
#include "tunnel/ws/web_socket_channel.h"

namespace hippy::devtools {

using asio::ip::tcp;
using WebSocketChannel = hippy::devtools::WebSocketChannel;

std::shared_ptr<WebSocketChannel> ws_;

void WsClient::Connect(const std::string &url, WReceiveDataHandler handler) {
  data_handler_ = handler;
  ws_ = std::make_shared<WebSocketChannel>(url);
  ws_->Connect([WEAK_THIS](const std::string& msg, int flag) {
    if (flag == hippy::devtools::kTaskFlag) {
      DEFINE_AND_CHECK_SELF(WsClient)
      self->HandleReceiveData(msg);
    }
  }, nullptr);
  
  std::string cmdArray[] = {
//    "{\"id\":1,\"method\":\"Network.enable\",\"params\":{\"maxPostDataSize\":65536,\"reportDirectSocketTraffic\":true}}",
//    "{\"id\":2,\"method\":\"Network.setAttachDebugStack\",\"params\":{\"enabled\":true}}",
//    "{\"id\":3,\"method\":\"Runtime.enable\",\"params\":{}}",
//    "{\"id\":4,\"method\":\"Debugger.enable\",\"params\":{\"maxScriptsCacheSize\":10000000}}",
//    "{\"id\":5,\"method\":\"Debugger.setPauseOnExceptions\",\"params\":{\"state\":\"none\"}}",
//    "{\"id\":6,\"method\":\"Debugger.setAsyncCallStackDepth\",\"params\":{\"maxDepth\":32}}",
//    "{\"id\":7,\"method\":\"Profiler.enable\",\"params\":{}}",
//    "{\"id\":8,\"method\":\"Network.clearAcceptedEncodingsOverride\",\"params\":{}}",
//    "{\"id\":9,\"method\":\"Debugger.setBlackboxPatterns\",\"params\":{\"patterns\":[\"/node_modules/|^node:\"],\"skipAnonymous\":false}}",
    "{\"id\":10,\"method\":\"Runtime.runIfWaitingForDebugger\",\"params\":{}}",
  };
  int len = sizeof(cmdArray) / sizeof(cmdArray[0]);
  FOOTSTONE_LOG(INFO) << "xxx hippy, send cmd count: " << len;
  for (int i = 0; i < len; i++) {
    ws_->Send(cmdArray[i]);
  }
}

void WsClient::Send(const std::string &data) {
  ws_->Send(data);
}

void WsClient::HandleReceiveData(const std::string& msg) {
  FOOTSTONE_LOG(INFO) << "xxx hippy, ws recv data: " << msg;
  
  data_handler_(msg);

  // {"method":"Debugger.paused","params":{"callFrames":[{"callFrameId":"6092848838021977782.1.0","functionName":"","functionLocation":{"scriptId":"3","lineNumber":0,"columnNumber":0},"location":{"scriptId":"3","lineNumber":0,"columnNumber":0},"url":"","scopeChain":[{"type":"global","object":{"type":"object","className":"global","description":"global","objectId":"6092848838021977782.1.1"}}],"this":{"type":"object","className":"global","description":"global","objectId":"6092848838021977782.1.2"},"canBeRestarted":true}],"reason":"Break on debugger attached","hitBreakpoints":[]}}

  std::string method;
  std::string scriptId;
  nlohmann::json data_json = nlohmann::json::parse(msg, nullptr, false);
  if (data_json.contains("method")) {
    method = data_json["method"].get<std::string>();
  }
  if (data_json.contains("scriptId")) {
    scriptId = data_json["scriptId"].get<std::string>();
    FOOTSTONE_LOG(INFO) << "xxx hippy, recv, method: " << method << ", scriptId: " << scriptId;
  }
  
//  if (method == "Debugger.paused") {
//    std::string cmdArray[] = {
//      "{\"id\":11,\"method\":\"Debugger.getScriptSource\",\"params\":{\"scriptId\":\"3\"}}",
//      "{\"id\":13,\"method\":\"Debugger.resume\",\"params\":{\"terminateOnResume\":false}}",
//    };
//    int len = sizeof(cmdArray) / sizeof(cmdArray[0]);
//    for (int i = 0; i < len; i++) {
//      ws_->Send(cmdArray[i]);
//    }
//  }
}

} // namespace hippy::devtools
