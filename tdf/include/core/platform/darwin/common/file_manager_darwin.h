#pragma once

#import <Foundation/Foundation.h>
#include "core/platform/common/file_manager.h"

namespace tdfcore {
class FileManagerDarwin : public FileManager {
 public:
  ~FileManagerDarwin() override = default;

  FileManagerDarwin();

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
  NSFileManager *file_manager_;
};
}  // namespace tdfcore
