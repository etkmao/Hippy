//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <string>

namespace tdfcore {

#define MAKE_DEFAULT_THREAD_RUNNER_NAME(thread_id) ("thread#" + std::to_string(thread_id))

using ThreadId = uint64_t;

/**
 * @brief 线程信息
 */
struct ThreadInfo {
  // 线程id
  ThreadId thread_id;
  // 对应的TaskRunner名称
  std::string runner_name;

  explicit ThreadInfo(const ThreadId thread_id = 0, const std::string &runner_name = "")
      : thread_id(thread_id), runner_name(runner_name) {}
};

}  // namespace tdfcore
