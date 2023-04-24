//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#import <Cocoa/Cocoa.h>
#include <vector>
#include "core/platform/common/file_picker.h"

namespace tdfcore {

class FilePickerMacOS final : public FilePicker {
 public:
  void PickFiles(const PickFileCallback &callback, FileType file_type = FileType::kAny, bool allow_multiple = false,
                   bool with_data = false) override;
  void PickDirectoryPath(const PickDirectoryPathCallback &callback) override;
 private:
  NSArray *GetAllowFileTypes(FileType file_type);
  std::vector<FileInfo> GetFileInfos(NSArray<NSURL *> *URLs, bool with_data);
};

}  // namespace tdfcore
