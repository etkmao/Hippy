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

#include "modules/network/websocket.h"

#include <atomic>

#include "footstone/logging.h"
#include "footstone/task_runner.h"
#include "footstone/worker_manager.h"

std::atomic<uint32_t> global_websocket_id{0};

constexpr char kSocketUrl[] = "url";
constexpr char kHeaders[] = "headers";
constexpr char kWebsocketId[] = "id";
constexpr char kSendData[] = "data";
constexpr char kCode[] = "code";
constexpr char kReason[] = "reason";

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

void Websocket::Initial() {}

Websocket::RetCode Websocket::ParseRequest(const footstone::value::HippyValue& request, std::string& url,
                                           std::unordered_map<std::string, std::string>& headers) {
  footstone::value::HippyValue::HippyValueArrayType array;
  bool ret = request.ToArray(array);
  if (!ret) return RetCode::Failed;
  if (array.size() != 1) return RetCode::Failed;
  footstone::value::HippyValue::HippyValueObjectType object;
  ret = array[0].ToObject(object);
  if (!ret) return RetCode::Failed;

  if (object.find(kSocketUrl) == object.end()) return RetCode::UrlError;
  ret = object.find(kSocketUrl)->second.ToString(url);
  if (!ret) return RetCode::UrlError;

  footstone::value::HippyValue::HippyValueObjectType hippy_headers;
  if (object.find(kHeaders) == object.end()) return RetCode::HeaderError;
  ret = object.find(kHeaders)->second.ToObject(hippy_headers);
  if (!ret) return RetCode::HeaderError;

  for (const auto& h : hippy_headers) {
    std::string value;
    if (h.second.IsNumber()) {
      double d;
      h.second.ToDouble(d);
      headers.insert({h.first, std::to_string(d)});
    } else if (h.second.IsBoolean()) {
      bool b;
      h.second.ToBoolean(b);
      headers.insert({h.first, std::to_string(b)});
    } else if (h.second.IsString()) {
      std::string s;
      h.second.ToString(s);
      headers.insert({h.first, s});
    } else {
      FOOTSTONE_DLOG(INFO) << "Unsupport Request Header!!!";
    }
  }

  return RetCode::Success;
}

Websocket::RetCode Websocket::ParseRequest(const footstone::value::HippyValue& request, int32_t& websocket_id,
                                           std::string& data) {
  footstone::value::HippyValue::HippyValueArrayType array;
  bool ret = request.ToArray(array);
  if (!ret) return RetCode::Failed;
  if (array.size() != 1) return RetCode::Failed;
  footstone::value::HippyValue::HippyValueObjectType object;
  ret = array[0].ToObject(object);
  if (!ret) return RetCode::Failed;

  if (object.find(kWebsocketId) == object.end()) return RetCode::Failed;
  ret = object.find(kWebsocketId)->second.ToInt32(websocket_id);
  if (!ret) return RetCode::Failed;

  if (object.find(kSendData) == object.end()) return RetCode::Failed;
  ret = object.find(kSendData)->second.ToString(data);
  if (!ret) return RetCode::Failed;

  return RetCode::Success;
}

Websocket::RetCode Websocket::ParseRequest(const footstone::value::HippyValue& request, int32_t& websocket_id,
                                           int32_t& code, std::string& reason) {
  footstone::value::HippyValue::HippyValueArrayType array;
  bool ret = request.ToArray(array);
  if (!ret) return RetCode::Failed;
  if (array.size() != 1) return RetCode::Failed;
  footstone::value::HippyValue::HippyValueObjectType object;
  ret = array[0].ToObject(object);
  if (!ret) return RetCode::Failed;

  if (object.find(kWebsocketId) == object.end()) return RetCode::Failed;
  ret = object.find(kWebsocketId)->second.ToInt32(websocket_id);
  if (!ret) return RetCode::Failed;

  if (object.find(kCode) == object.end()) return RetCode::Failed;
  if (object.find(kCode)->second.IsInt32()) {
    object.find(kCode)->second.ToInt32(code);
  } else {
    code = 0;
  }

  if (object.find(kReason) == object.end()) return RetCode::Failed;
  if (object.find(kReason)->second.IsString()) {
    object.find(kReason)->second.ToString(reason);
  } else {
    reason = "";
  }

  return RetCode::Success;
}

Websocket::~Websocket() {
  for (const auto& [id, client] : websocket_client_map_) {
    client->Disconnect(0, "client close");
  }
}

void Websocket::Connect(const std::shared_ptr<Scope>& scope, const footstone::value::HippyValue& request,
                        std::function<void(footstone::value::HippyValue)> callback) {
  std::unordered_map<std::string, std::string> headers;
  std::string url;
  RetCode ret_code = ParseRequest(request, url, headers);
  if (ret_code != RetCode::Success) {
    footstone::value::HippyValue::HippyValueObjectType callback_object;
    callback_object.insert({kCode, footstone::value::HippyValue(-1)});

    if (ret_code == RetCode::Failed) {
      callback_object.insert({kReason, footstone::value::HippyValue("Invalid connect param")});
    } else if (ret_code == RetCode::UrlError) {
      callback_object.insert({kReason, footstone::value::HippyValue("Invalid url for websocket")});
    } else if (ret_code == RetCode::HeaderError) {
      callback_object.insert({kReason, footstone::value::HippyValue("Invalid header for websocket")});
    }
    callback(footstone::value::HippyValue(callback_object));
  }

  uint32_t id = global_websocket_id.fetch_add(1);
  auto client = std::make_shared<WebsocketClient>(id, url, headers);
  auto listener = std::make_shared<WebsocketEventListener>(scope, id);
  client->SetEventListener(listener);
  websocket_client_map_.insert({id, client});
  client->SetCloseCallback([WEAK_THIS](uint32_t websocket_id) {
    DEFINE_AND_CHECK_SELF(Websocket)
    self->websocket_client_map_.erase(websocket_id);
  });

  client->Connect();
  footstone::value::HippyValue::HippyValueObjectType callback_object;
  callback_object.insert({kCode, footstone::value::HippyValue(0)});
  callback_object.insert({kReason, footstone::value::HippyValue("")});
  callback_object.insert({kWebsocketId, footstone::value::HippyValue(id)});
  callback(footstone::value::HippyValue(callback_object));
}

void Websocket::Disconnect(const footstone::value::HippyValue& request) {
  int32_t websocket_id;
  int32_t code;
  std::string reason;
  RetCode ret_code = ParseRequest(request, websocket_id, code, reason);
  auto iter = websocket_client_map_.find(static_cast<uint32_t>(websocket_id));
  if (iter == websocket_client_map_.end()) {
    FOOTSTONE_DLOG(INFO) << "websocket client not found !!! websocket id = " << websocket_id;
    return;
  }

  auto client = iter->second;
  client->Disconnect(code, reason);
  return;
}

void Websocket::Send(const footstone::value::HippyValue& request) {
  int32_t websocket_id;
  std::string data;
  RetCode ret_code = ParseRequest(request, websocket_id, data);
  if (ret_code != RetCode::Success) {
    FOOTSTONE_DLOG(INFO) << "websocket send message parse request failed !!!";
    return;
  }

  auto iter = websocket_client_map_.find(static_cast<uint32_t>(websocket_id));
  if (iter == websocket_client_map_.end()) {
    FOOTSTONE_DLOG(INFO) << "websocket client not found !!! websocket id = " << websocket_id;
    return;
  }
  auto client = iter->second;
  client->Send(data);
}

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
