//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <filesystem>
#include "core/common/object.h"
#include "core/platform/common/file_manager.h"

namespace tdfcore {

/**
 * @brief Win平台文件管理类
 */
class FileManagerWindows : public FileManager {
 public:
  FileManagerWindows();
  ~FileManagerWindows() override = default;

  /**
   * @brief 文件是否存在
   * @param path 文件路径
   * @return 是否存在
   */
  bool FileExists(const std::string &path) const override;

  /**
   * @brief 创建文件夹
   * @param path 路径
   * @return 是否成功
   */
  bool TCreateDirectory(const std::string &path) const override;

  /**
   * @brief 删除文件或文件夹
   * @param path 路径
   * @return 是否成功
   */
  bool TDeleteFile(const std::string &path) const override;

  /**
   * @brief 获取文件信息
   * @param path 路径
   * @return 文件信息
   */
  std::vector<FileInfo> GetFileInfos(const std::string &path) const override;

  /**
   * @brief 获取Asset文件
   */
  std::shared_ptr<TData> GetAssetsFile(const std::string &file_name) const override;

 private:
  FileInfo GetFileInfo(const std::filesystem::path &file_path) const;
  std::string executable_directory_;
};
}  // namespace tdfcore
