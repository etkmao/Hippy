//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <string_view>
#include <vector>
#include "core/common/object.h"
#include "core/platform/common/file_manager.h"

namespace tdfcore {

enum class FileType {
  kAny,
  kMedia,
  kImage,
  kVideo,
  kAudio,
};

class FilePicker : public Object, public std::enable_shared_from_this<FilePicker> {
 public:
  /// 选择文件回调函数
  using PickFileCallback = std::function<void(const std::vector<FileInfo> &)>;
  /// 选择目录回调函数
  using PickDirectoryPathCallback = std::function<void(const std::u16string &)>;
  /***
   * @brief 文件选择
   * @param callback 文件选取之后的回调函数
   * @param file_type 文件类型
   * @param allow_multiple 是否允许多选
   * @param with_data 返回把选择的文件加载到内存，如果设置为true则会把文件加载到FileInfo的buffer当中
   */
  virtual void PickFiles(const PickFileCallback &callback, FileType file_type = FileType::kAny,
                         bool allow_multiple = false, bool with_data = false) = 0;

  /***
   * @brief 目录选择
   * @param callback 目录选择完回调函数
   */
  virtual void PickDirectoryPath(const PickDirectoryPathCallback &callback) = 0;

 protected:
  PickFileCallback pick_file_callback_;
  PickDirectoryPathCallback pick_directory_path_callback_;
};

}  // namespace tdfcore
