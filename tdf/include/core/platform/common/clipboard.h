//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include "core/common/memory.h"
#include "core/common/object.h"
namespace tdfcore {
class ClipboardData;

/**
 * @brief 剪切板格式类型
 */
enum ClipboardFormatType { kTextPlain };

/**
 * @brief 剪切板
 */
class Clipboard : public Object {
 public:
  /**
   * @brief 设置剪切板数据
   * @param data 剪切板数据
   * @return 是否设置成功
   */
  virtual bool SetData(const std::shared_ptr<ClipboardData>& data) = 0;

  /**
   * @brief 获取剪贴板数据
   * @param formatType 数据格式
   * @return 剪切板数据
   */
  virtual std::shared_ptr<ClipboardData> GetData(ClipboardFormatType formatType) = 0;

  /**
   * @brief 剪切板数据是否为空
   * @param formatType 数据格式
   * @return 是否为空
   */
  bool IsDataEmpty(ClipboardFormatType formatType);
};

/**
 * @brief 剪切板数据类
 */
class ClipboardData : public Object {
 public:
  /**
   * @brief 剪切板数据格式类型
   * @return 格式类型
   */
  virtual ClipboardFormatType FormatType() const = 0;
};

/**
 * @brief 字符串格式剪切板数据
 */
class ClipboardTextData : public ClipboardData {
 public:
  ~ClipboardTextData() override = default;
  /**
   * @brief 获取剪切板数据
   * @return 剪切板数据
   */
  std::u16string Text() const { return text_; }

  /**
   * @brief 剪切板数据格式类型
   * @return 格式类型
   */
  ClipboardFormatType FormatType() const override { return ClipboardFormatType::kTextPlain; }

 protected:
  explicit ClipboardTextData(const std::u16string& text = u"");

 private:
  std::u16string text_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
