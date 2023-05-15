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

#include "modules/network/websocket_listener.h"

#include "driver/js_driver_utils.h"
#include "footstone/deserializer.h"
#include "footstone/string_view.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

constexpr char kEventModuleName[] = "EventDispatcher";
constexpr char kEventMethodName[] = "receiveNativeEvent";
constexpr char kWebsocketEventName[] = "hippyWebsocketEvents";
constexpr char kEventTypeOpen[] = "onOpen";
constexpr char kEventTypeClose[] = "onClose";
constexpr char kEventTypeError[] = "onError";
constexpr char kEventTypeMessage[] = "onMessage";
constexpr char kReason[] = "reason";
constexpr char kData[] = "data";
constexpr char kType[] = "type";
constexpr char kCode[] = "code";

WebsocketEventListener::WebsocketEventListener(const std::shared_ptr<Scope>& scope, uint32_t websocket_client_id)
    : weak_scope_(scope), websocket_client_id_(websocket_client_id) {}

void WebsocketEventListener::Open() { SendWebsocketEvent(kEventTypeOpen, footstone::HippyValue::Null()); }

void WebsocketEventListener::Close(const int32_t code, const std::string& reason) {
  footstone::HippyValue::HippyValueObjectType object;
  object.insert({kCode, footstone::HippyValue(code)});
  object.insert({kReason, footstone::HippyValue(reason)});
  SendWebsocketEvent(kEventTypeClose, footstone::HippyValue(object));
}

void WebsocketEventListener::Error(const std::string& error_message) {
  footstone::HippyValue::HippyValueObjectType object;
  object.insert({kReason, footstone::HippyValue(error_message)});
  SendWebsocketEvent(kEventTypeError, footstone::HippyValue(object));
}

void WebsocketEventListener::Message(const std::string& recv_message) {
  footstone::HippyValue::HippyValueObjectType object;
  object.insert({kData, footstone::HippyValue(recv_message)});
  object.insert({kType, footstone::HippyValue("text")});
  SendWebsocketEvent(kEventTypeMessage, footstone::HippyValue(object));
}

void WebsocketEventListener::SendWebsocketEvent(std::string event_type, footstone::HippyValue data) {
  footstone::Serializer serializer;
  serializer.WriteHeader();
  footstone::HippyValue::HippyValueObjectType object;
  object.insert({"moduleName", footstone::HippyValue(kEventModuleName)});
  object.insert({"methodName", footstone::HippyValue(kEventMethodName)});
  footstone::HippyValue::HippyValueObjectType params;
  params.insert({"id", footstone::HippyValue(websocket_client_id_)});
  params.insert({"type", footstone::HippyValue(event_type)});
  params.insert({"data", data});
  footstone::HippyValue::HippyValueArrayType array;
  array.push_back(footstone::HippyValue(kWebsocketEventName));
  array.push_back(footstone::HippyValue(params));
  object.insert({"params", footstone::HippyValue(array)});

  serializer.WriteValue(footstone::HippyValue(object));
  std::pair<uint8_t*, size_t> buffer = serializer.Release();
  JsDriverUtils::byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
  auto call_js_callback = [](CALL_FUNCTION_CB_STATE state, const footstone::string_view& msg) {
    FOOTSTONE_DLOG(INFO) << msg;
  };
  auto on_js_runner = []() {};
  auto scope = weak_scope_.lock();
  if (scope) JsDriverUtils::CallJs(u"callJsModule", scope, call_js_callback, buffer_data, on_js_runner);
}

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
