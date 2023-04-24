//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "core/common/object.h"

namespace tdfcore {

// TODO(dexterzhao): std::filesystem在iOS的高版本上才有实现，除此以外，暂时没有比较好的方法做跨平台的文件路径拼接
#if defined(_WIN32)
constexpr const char *kFilePathSeperator = "\\";
#else
constexpr const char *kFilePathSeperator = "/";
#endif

/**
 * @brief 文件相关的工具类
 */
class FileUtility {
 public:
  /**
   * @brief 以文本形式读取文件的全部内容
   * @param file_path 文件路径
   */
  static std::string ReadStringFromFile(const std::string &file_path);

  /**
   * @brief 将文本内容写入文件
   * @param file_path 文件路径
   * @param content 内容
   */
  static void WriteStringToFile(const std::string &file_path, const std::string &content);

  /**
   * @brief 将二进制内容写入文件
   * @param file_path 文件路径
   * @param bytes 内容
   * @param length 内容的长度
   */
  static void WriteBytesToFile(const std::string &file_path, const char *bytes, size_t length);

  /**
   * @brief 文件路径拼接
   * @param components 路径的组成列表
   */
  static std::string JoinPathComponents(const std::vector<std::string> &components);

  /**
   * @brief 获取文件路径中的最后一段
   * @param path 路径
   */
  static std::string GetLastPathComponent(const std::string &path);

  /**
   * @brief 除去文件路径中的最后一段而剩下的路径
   * @param components 路径
   */
  static std::string GetPathByRemovingLastPathComponent(const std::string &path);
};

}  // namespace tdfcore
