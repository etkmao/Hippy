//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/common/object.h"
#include "core/platform/common/file_manager.h"

namespace tdfcore {

class FileManagerLinux : public FileManager {
 public:
  bool FileExists(const std::string &path) const override;
  bool TCreateDirectory(const std::string &path) const override;
  bool TDeleteFile(const std::string &path) const override;
  std::vector<FileInfo> GetFileInfos(const std::string &path) const override;
  // TODO(vimerzhao): 这个接口设计的不太好，不是所有平台都有 Assets 的概念
  std::shared_ptr<TData> GetAssetsFile(const std::string &file_name) const override;
};
}  // namespace tdfcore
