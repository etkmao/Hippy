//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <string>

namespace tdfcore {
inline namespace log {
enum LogSeverity {
  TDF_LOG_DEBUG,
  TDF_LOG_INFO = 0,
  TDF_LOG_WARNING,
  TDF_LOG_ERROR,
  TDF_LOG_FATAL,
  TDF_LOG_NUM_SEVERITIES
};

// One of the Windows headers defines ERROR to 0. This makes the token
// concatenation in BASE_LOG(ERROR) to resolve to LOG_0. We define this back to
// the appropriate log level.
#ifdef _WIN32
#define LOG_0 LOG_ERROR
#endif

// LOG_DFATAL is LOG_FATAL in debug mode, ERROR in normal mode
#ifdef NDEBUG
constexpr LogSeverity TDF_LOG_DFATAL = TDF_LOG_ERROR;
#else
constexpr LogSeverity TDF_LOG_DFATAL = TDF_LOG_FATAL;
#endif

struct LogSettings {
  LogSeverity min_log_level = TDF_LOG_INFO;
};

void SetLogSettings(const LogSettings& settings);

LogSettings GetLogSettings();

int GetMinLogLevel();
}
}  // namespace tdfcore