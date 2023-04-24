//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
#include "core/common/data.h"
#include "core/common/macro.h"

namespace tdfcore {
struct FileInfo {
  std::string filepath;
  std::string filename;
  int64_t size;
  int64_t modification_timestamp;
  std::vector<uint8_t> buffer;

  FileInfo(const std::string &filepath, int64_t size, int64_t modification_timestamp)
      : filepath(filepath), size(size), modification_timestamp(modification_timestamp) {}
  FileInfo(const std::string &filepath, const std::string &filename, const std::vector<uint8_t> &buffer)
      : filepath(filepath), filename(filename), buffer(buffer) {}
};

class FileManager : public Object {
 public:
  ~FileManager() override = default;

  /**
   * @brief 文件是否存在
   * @param path 文件路径
   * @return 是否存在
   */
  virtual bool FileExists(const std::string &path) const = 0;

  /**
   * @brief 创建文件夹
   * @param path 路径
   * @return 是否成功
   */
  virtual bool TCreateDirectory(const std::string &path) const = 0;

  /**
   * @brief 删除文件
   * @param path 路径
   * @return 是否成功
   */
  virtual bool TDeleteFile(const std::string &path) const = 0;

  /**
   * @brief 获取文件信息
   * @param path 路径
   * @return 文件信息，若`path`是文件size为1，若`path`是文件夹size为文件夹中文件的数量
   */
  virtual std::vector<FileInfo> GetFileInfos(const std::string &path) const = 0;

  /**
   * @brief 获取Assets路径下的文件
   * @param file_name 文件名
   * @return skdata 数据 buffer
   */
  virtual std::shared_ptr<TData> GetAssetsFile(const std::string &file_name) const = 0;
};
}  // namespace tdfcore
