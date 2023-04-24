//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/data.h"
#include "core/engine/schedule/task_runner.h"

namespace tdfcore {

/**
 * @brief 图片加载错误枚举
 */
enum class ImageLoadError {
  // 无错误
  kNone,

  // 取消
  kCanceled,

  // 加载过程中遭遇失败
  kLoadFailed,
};

/**
 * @brief 图片文件数据加载器
 */
class ImageLoader : public Object {
 public:
  /**
   * @brief 图片加载进度回调函数
   * @param progress 加载进度
   */
  using ProgressCallback = std::function<void(float progress)>;

  /**
   * @brief 图片加载完成回调函数
   * @param data 图片文件数据
   * @param error 错误
   */
  using FinishCallback = std::function<void(const std::shared_ptr<TData> &data, ImageLoadError error)>;

  /**
   * @brief 加载图片
   * @param url 图片url
   * @param progress_callback 图片加载进度回调
   * @param finish_callback 图片加载完成回调
   * @return Task
   */
  virtual std::shared_ptr<Task> Load(const std::string &url, const ProgressCallback &progress_callback,
                                     const FinishCallback &finish_callback) = 0;
};

}  // namespace tdfcore
