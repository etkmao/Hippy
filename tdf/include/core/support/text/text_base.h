//
// Copyright © 2020 com.tencent. All rights reserved.
//

#pragma once
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "core/common/object.h"
#include "core/common/point.h"
#include "core/common/color.h"
#include "core/common/m33.h"

namespace tdfcore {
/**
 * @brief 编码方式
 */
enum class TextEncoding {
  kUTF8,
  kUTF16,
  kUTF32,
  kGlyphID,
};

/**
 * @brief 字符水平对齐方式
 */
enum class TextBaseline {
  /// 字形底部作为水平线
  kAlphabetic,
  /// 表意字符作为水平线
  kIdeographic
};
/**
 * @brief 文本方向
 */
enum class TextAffinity {
  /// 上游方向
  kUpstream,
  /// 下游方向
  kDownstream
};
/**
 * @brief 文本益处部分处理方式
 */
enum class TextOverflow {
  /// 直接裁剪
  kClip,
  /// 透明渐变处理
  kFade,
  /// 用ellipsis表示超出部分
  kEllipsis,
  /// 直接渲染出来
  kVisible
};
/**
 * @brief 一行或多行文本测量宽度的不同方式
 */
enum class TextWidthBasis {
  /// 多行文本的将占用父类给出的全部宽度，针对一行文本只需要包含文本所需要的最小宽度
  kParent,
  /// 宽度刚好可以容纳最长的行
  kLongestLine,
};

/**
 * @brief 比较结果
 */
enum class TextRenderComparison {
  /// 两个对象完全一致
  kIdentical,
  /// 两个对象基本一致，不影响Layout
  kMetadata,
  /// 两个对象不一致，需要重绘
  kPaint,
  /// 两个对象不一致，需要重新布局
  kLayout
};

/**
 * @brief 文本轴方向
 */
enum class TextAxis {
  kHorizontal,  // 横向
  kVertical,    // 竖向
};

/**
 * @brief 文本方向
 */
enum class TextDirection {
  kLtr,
  kRtl,
};

enum class HorizontalAlign {
  kLeft,     // 左对齐
  kCenter,   // 居中对齐
  kRight,    // 右对齐
  kJustify,  // 分散对齐
};

// 垂直对齐方式
enum class VerticalAlign {
  kTop,      // 居上
  kCenter,   // 居中
  kBottom    // 居下
};

/**
 * @brief 文字位置类
 */
class TextPosition {
 public:
  /**
   * @brief 构造函数
   * @param offset 字形位置
   * @param affinity 方向
   */
  explicit TextPosition(int64_t offset = 0, TextAffinity affinity = TextAffinity::kDownstream)
      : offset_(offset), affinity_(affinity) {}

  constexpr int64_t GetOffset() const { return offset_; }
  void SetOffset(int64_t offset) { offset_ = offset; }

  constexpr TextAffinity GetAffinity() const { return affinity_; }
  void SetAffinity(TextAffinity textAffinity) { affinity_ = textAffinity; }

  constexpr bool operator==(const TextPosition &other) const {
    return offset_ == other.offset_ && affinity_ == other.affinity_;
  }
  constexpr bool operator>(const TextPosition &other) const {
    assert(affinity_ == other.affinity_);
    return offset_ > other.offset_;
  }
  constexpr bool operator<(const TextPosition &other) const {
    assert(affinity_ == other.affinity_);
    return offset_ < other.offset_;
  }

 private:
  int64_t offset_;
  TextAffinity affinity_;
};
/**
 * @brief 文字范围类
 */
struct TextRange {
 public:
  /**
   * @brief 一个抽象的文本范围[start, end)
   * @param start 起始位置，必须大于或等于-1
   * @param end 结束位置，必须大于或等于-1
   */
  TextRange(int64_t start, int64_t end);

  static TextRange MakeEmpty();
  static TextRange MakeCollapsed(int offset);

  constexpr bool operator==(const TextRange &other) const { return start_ == other.start_ && end_ == other.end_; }

  constexpr int64_t GetStart() const { return start_; }
  constexpr int64_t GetEnd() const { return end_; }
  constexpr int64_t GetSize() const { return end_ - start_; }
  void SetStart(int64_t start);
  void SetEnd(int64_t end);

  constexpr bool IsValid() const { return (start_ >= 0) && (end_ >= 0); }
  constexpr bool IsCollapsed() const { return start_ == end_; }
  constexpr bool IsNormalized() const { return end_ >= start_; }

  std::u16string GetTextInside(const std::u16string &text) const;
  std::u16string GetTextBefore(const std::u16string &text) const;
  std::u16string GetTextAfter(const std::u16string &text) const;

  constexpr bool operator==(TextRange &other) const { return start_ == other.start_ && end_ == other.end_; }

 private:
  int64_t start_;
  int64_t end_;
};
/**
 * @brief 文字选中区域
 */
struct TextSelection : public TextRange {
 public:
  /**
   * @brief 构造函数
   * @param base 选中区域最初的位置，可以><= extent
   * @param extent 选中区域的终止的位置
   * @param text_affinity 方向
   */
  explicit TextSelection(int64_t base = -1, int64_t extent = -1,
                         TextAffinity text_affinity = TextAffinity::kDownstream);

  static TextSelection MakeCollapsed(int64_t offset, TextAffinity text_affinity = TextAffinity::kDownstream);

  static TextSelection MakePosition(TextPosition position);

  constexpr bool IsDirectional() const { return false; }

  TextPosition GetBasePosition() const;
  TextPosition GetExtentPosition() const;
  constexpr TextAffinity GetAffinity() const { return text_affinity_; }

  constexpr bool operator==(const TextSelection &other) const {
    return base_offset_ == other.base_offset_ && extent_offset_ == other.extent_offset_ &&
           text_affinity_ == other.text_affinity_;
  }

  constexpr bool operator!=(const TextSelection &other) const { return !(*this == other); }

 private:
  int64_t base_offset_;
  int64_t extent_offset_;
  TextAffinity text_affinity_;
};
/**
 * @brief 文字在选中区域的坐标
 */
struct TextSelectionPoint {
  TScalar x;
  TScalar y_top;
  TScalar y_bottom;
};

}  // namespace tdfcore
