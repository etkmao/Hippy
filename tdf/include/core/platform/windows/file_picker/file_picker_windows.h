//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include "core/platform/common/file_picker.h"

namespace tdfcore {

class FilePickerWindows final : public FilePicker {
 public:
  void PickFiles(const PickFileCallback &callback, FileType file_type = FileType::kAny, bool allow_multiple = false,
                 bool with_data = false) override;
  void PickDirectoryPath(const PickDirectoryPathCallback &callback) override;

 private:
  void *GetAllowFileTypes(FileType file_type);
  std::vector<FileInfo> GetFileInfos(const std::vector<std::string> &file_path_vector, bool with_data);
};

}  // namespace tdfcore
