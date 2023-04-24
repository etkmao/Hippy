//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/memory.h"
#include "tdfui/image/image_loader.h"

namespace tdfcore {

class FileImageLoader : public ImageLoader {
 public:
  ~FileImageLoader() override = default;

  /**
   * @brief FileImageLoader能够加载具有该scheme的图片
   */
  static constexpr const char *kScheme = "file";

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
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
