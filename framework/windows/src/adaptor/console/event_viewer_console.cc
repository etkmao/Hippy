
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

#include "adaptor/console/event_viewer_console.h"

#include <iostream>

#include "footstone/logging.h"
#include "footstone/macros.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

EventViewerConsole::EventViewerConsole(std::string app_name, bool debug)
    : app_name_(std::move(app_name)), debug_(debug) {}

bool EventViewerConsole::Initial() {
  footstone::log::LogMessage::InitializeDelegate(
      [WEAK_THIS](const std::ostringstream& stream, footstone::log::LogSeverity severity) {
        DEFINE_AND_CHECK_SELF(EventViewerConsole)
        std::string str = stream.str();
        if (severity == footstone::log::LogSeverity::TDF_LOG_INFO) {
          self->Info(str);
        } else if (severity == footstone::log::LogSeverity::TDF_LOG_WARNING) {
          self->Warn(str);
        } else if (severity == footstone::log::LogSeverity::TDF_LOG_ERROR) {
          self->Error(str);
        } else if (severity == footstone::log::LogSeverity::TDF_LOG_FATAL) {
          self->Fatal(str);
        }
      });
  return true;
}

void EventViewerConsole::Info(const std::string& message) { WriteLog(message, EVENTLOG_INFORMATION_TYPE); }

void EventViewerConsole::Warn(const std::string& message) { WriteLog(message, EVENTLOG_WARNING_TYPE); }

void EventViewerConsole::Error(const std::string& message) { WriteLog(message, EVENTLOG_ERROR_TYPE); }

void EventViewerConsole::Fatal(const std::string& message) { WriteLog(message, EVENTLOG_ERROR_TYPE); }

void EventViewerConsole::WriteLog(const std::string message, WORD event_type) {
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

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
