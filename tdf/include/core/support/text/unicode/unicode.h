//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/object.h"
#include "core/support/font/types.h"
#include "core/common/data.h"
#include "core/support/text/text_base.h"
namespace tdfcore {

enum class LineBreakType {
  kSoftLineBreak = 0,
  kHardLineBreak = 100,
};

// 打断类型
enum class BreakType { kWords, kGraphemes, kLines };

struct LineBreakBefore {
  LineBreakBefore(size_t pos, LineBreakType breakType) : pos(pos), breakType(breakType) {}
  size_t pos;
  LineBreakType breakType;
};

// Bidi范围，表示这个范围内Bidi level是一样的
struct BidiRegion {
  BidiRegion(size_t start, size_t end, uint8_t level) : start(start), end(end), level(level) {}
  size_t start;
  size_t end;
  uint8_t level;
};

// Bidi迭代器
class BidiIterator {
 public:
  virtual ~BidiIterator() = default;
  virtual size_t GetLength() = 0;
  virtual uint8_t GetLevelAt(size_t) = 0;
  static void ReorderVisual(const uint8_t runLevels[], int levelsCount, int32_t logicalFromVisual[]);
};

// 类型打断迭代器
class BreakIterator {
 public:
  virtual ~BreakIterator() = default;
  virtual int32_t First() = 0;
  virtual int32_t Current() = 0;
  virtual int32_t Next() = 0;
  virtual int32_t Preceding(int32_t offset) = 0;
  virtual int32_t Following(int32_t offset) = 0;
  virtual int32_t RuleStatus() = 0;
  virtual bool IsDone() = 0;
  virtual bool SetText(const char utftext8[], int utf8Units) = 0;
  virtual bool SetText(const char16_t utftext16[], int utf16Units) = 0;
};

class TUnicode {
 public:
  static std::shared_ptr<TUnicode> Make();

  virtual ~TUnicode() = default;
  /**
   * @brief 判断是否是Control
   * @param utf8
   * @return
   */
  virtual bool IsControl(Unichar utf8) = 0;
  /**
   * @brief 判断unicode是否是whitespace
   * @param utf8
   * @return
   */
  virtual bool IsWhitespace(Unichar utf8) = 0;
  /**
   * @brief 判断unicode是否是Space
   * @param utf8
   * @return
   */
  virtual bool IsSpace(Unichar utf8) = 0;
  /**
   * @brief
   * @param utf8
   * @return
   */
  virtual bool IsHardBreak(Unichar utf8) = 0;
  /**
   * @brief
   * @param utf8
   * @return
   */
  virtual bool IsEmoji(Unichar utf8) = 0;
  /**
   * @brief 创建Bidi迭代器
   * @param text
   * @param count
   * @return
   */
  virtual std::unique_ptr<BidiIterator> MakeBidiIterator(const uint16_t text[], int count, TextDirection) = 0;
  /**
   * @brief 创建Bidi迭代器
   * @param text
   * @param count
   * @return
   */
  virtual std::unique_ptr<BidiIterator> MakeBidiIterator(const char text[], int count, TextDirection) = 0;
  /**
   * @brief 创建Break迭代器
   * @param locale
   * @param break_type 打断类型
   * @return
   */
  virtual std::unique_ptr<BreakIterator> MakeBreakIterator(const char locale[], BreakType break_type) = 0;
  /**
   * @brief 创建Break迭代器
   * @param type 打断类型
   * @return
   */
  virtual std::unique_ptr<BreakIterator> MakeBreakIterator(BreakType type) = 0;
  /**
   * @brief 获取bidi区域信息
   * @param utf8
   * @param count
   * @param dir
   * @param results
   * @return
   */
  virtual bool GetBidiRegions(const char utf8[], int count, TextDirection dir, std::vector<BidiRegion>& results) = 0;
  /**
   * @brief 获取字符串里面的硬换行
   * @param utf8
   * @param count
   * @param results
   * @return
   */
  virtual bool GetLineBreaks(const char utf8[], int count, std::vector<LineBreakBefore>& results) = 0;
  /**
   * @brief 获取字符串里面的单词
   * @param utf8
   * @param count
   * @param results
   * @return
   */
  virtual bool GetWords(const char utf8[], int count, std::vector<uint32_t>& results) = 0;
};

}  // namespace tdfcore
