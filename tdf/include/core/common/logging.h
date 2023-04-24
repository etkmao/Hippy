//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include <cassert>
#include <codecvt>
#include <functional>
#include <locale>
#include <sstream>
#include "core/common/log_settings.h"
#include "core/common/macro.h"

namespace tdfcore {
inline namespace log {
inline std::ostream& operator<<(std::ostream& stream, const std::string_view& str_view) {
  stream << str_view.data();
  return stream;
}

class LogMessageVoidify {
 public:
  void operator&(std::ostream&) {}
};

class LogMessage {
 public:
  LogMessage(LogSeverity severity, const char* file, int line, const char* condition);
  ~LogMessage();

  inline static void SetDelegate(const std::function<void(const std::ostringstream&, LogSeverity)>& delegate) {
    delegate_ = delegate;
  }

  inline static auto GetDelegate() { return delegate_; }

  std::ostringstream& stream() { return stream_; }

 private:
  static std::function<void(const std::ostringstream&, LogSeverity)> delegate_;

  std::ostringstream stream_;
  const LogSeverity severity_;
  const char* file_;
  const int line_;

  TDF_DISALLOW_COPY_AND_ASSIGN(LogMessage);
};

int GetVlogVerbosity();

bool ShouldCreateLogMessage(LogSeverity severity);
}  // namespace log
}  // namespace tdfcore

#define TDF_LOG_STREAM(severity) \
  ::tdfcore::LogMessage(::tdfcore::LogSeverity::severity, __FILE__, __LINE__, nullptr).stream()

#define TDF_LAZY_STREAM(stream, condition) !(condition) ? (void)0 : ::tdfcore::LogMessageVoidify() & (stream)

#define TDF_EAT_STREAM_PARAMETERS(ignored) \
  true || (ignored)                        \
      ? (void)0                            \
      : ::tdfcore::LogMessageVoidify() & ::tdfcore::LogMessage(::tdfcore::TDF_LOG_FATAL, 0, 0, nullptr).stream()

#define TDF_LOG_IS_ON(severity) (::tdfcore::ShouldCreateLogMessage(::tdfcore::LogSeverity::severity))

#define TDF_LOGD TDF_LAZY_STREAM(TDF_LOG_STREAM(TDF_LOG_DEBUG), TDF_LOG_IS_ON(TDF_LOG_DEBUG))

#define TDF_LOGE TDF_LAZY_STREAM(TDF_LOG_STREAM(TDF_LOG_ERROR), TDF_LOG_IS_ON(TDF_LOG_ERROR))

#define TDF_LOGI TDF_LAZY_STREAM(TDF_LOG_STREAM(TDF_LOG_INFO), TDF_LOG_IS_ON(TDF_LOG_INFO))

#define TDF_LOGW TDF_LAZY_STREAM(TDF_LOG_STREAM(TDF_LOG_WARNING), TDF_LOG_IS_ON(TDF_LOG_WARNING))

#define TDF_CHECK(condition)                                                                                \
  TDF_LAZY_STREAM(::tdfcore::LogMessage(::tdfcore::TDF_LOG_FATAL, __FILE__, __LINE__, #condition).stream(), \
                  !(condition))

#define TDF_VLOG_IS_ON(verbose_level) ((verbose_level) <= ::tdfcore::GetVlogVerbosity())

#define TDF_VLOG_STREAM(verbose_level) ::tdfcore::LogMessage(-verbose_level, __FILE__, __LINE__, nullptr).stream()

#define TDF_VLOG(verbose_level) TDF_LAZY_STREAM(TDF_BASE_VLOG_STREAM(verbose_level), TDF_VLOG_IS_ON(verbose_level))

#ifndef NDEBUG
#  define TDF_DLOGD TDF_LOGD
#  define TDF_DLOGE TDF_LOGE
#  define TDF_DLOGW TDF_LOGW
#  define TDF_DLOGI TDF_LOGI
#  define TDF_DCHECK(condition) TDF_CHECK(condition)
#else
#  define TDF_DLOGD TDF_EAT_STREAM_PARAMETERS(true)
#  define TDF_DLOGE TDF_EAT_STREAM_PARAMETERS(true)
#  define TDF_DLOGW TDF_EAT_STREAM_PARAMETERS(true)
#  define TDF_DLOGI TDF_EAT_STREAM_PARAMETERS(true)
#  define TDF_DCHECK(condition) TDF_EAT_STREAM_PARAMETERS(condition)
#endif

#define TDF_NOTREACHED() \
  do {                   \
    TDF_DCHECK(false);   \
    abort();             \
  } while (0)

#define TDF_NOTIMPLEMENTED() TDF_LOGE << "Not implemented in: " << __PRETTY_FUNCTION__

#define TDF_USE(expr) \
  do {                \
    (void)(expr);     \
  } while (0)
