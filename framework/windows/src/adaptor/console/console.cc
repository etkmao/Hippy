
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

#include "adaptor/console/console.h"

#include <iostream>

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

Console::Console(std::string app_name, bool debug) : app_name_(std::move(app_name)), debug_(debug) {}

bool Console::Initial() { return true; }

void Console::Info(const std::string& message) { WriteLog(message, EVENTLOG_INFORMATION_TYPE); }

void Console::Warn(const std::string& message) { WriteLog(message, EVENTLOG_WARNING_TYPE); }

void Console::Error(const std::string& message) { WriteLog(message, EVENTLOG_ERROR_TYPE); }

void Console::Fatal(const std::string& message) { WriteLog(message, EVENTLOG_ERROR_TYPE); }

void Console::WriteLog(const std::string message, WORD event_type) {
  if (debug_) {
    std::cout << message;
  } else {
    HANDLE handle = RegisterEventSource(NULL, message.c_str());
    if (handle != NULL) {
      const char* message_array[2] = {message.c_str(), NULL};
      ReportEvent(handle, event_type, 0, 0, NULL, 1, 0, message_array, NULL);
      DeregisterEventSource(handle);
    }
  }
}

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
