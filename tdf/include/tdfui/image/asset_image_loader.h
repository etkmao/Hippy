//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/schedule/thread_task_driver.h"
#include "core/platform/common/file_manager.h"
#include "tdfui/image/image_loader.h"

namespace tdfcore {

class AssetImageLoader : public ImageLoader {
 public:
  ~AssetImageLoader() override = default;

  /**
   * @brief 构造函数
   * @param file_manager 平台侧文件管理器
   */
  explicit AssetImageLoader(const std::shared_ptr<FileManager> &file_manager);

  /**
   * @brief AssetImageLoader能够加载具有该scheme的图片
   */
  static constexpr const char *kScheme = "asset";

  /**
   * @brief 加载图片
   * @param url 图片url
   * @param progress_callback 图片加载进度回调
   * @param finish_callback 图片加载完成回调
   * @return Task
   */
  std::shared_ptr<Task> Load(const std::string &url, const ProgressCallback &progress_callback,
                             const FinishCallback &finish_callback) override;

 private:
  std::shared_ptr<FileManager> file_manager_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
