//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include "core/common/point.h"
#include "core/common/rect.h"
#include "core/common/scalar.h"
#include "core/engine/graphics/paint.h"
#include "core/support/font/font.h"
#include "core/support/font/font_manager.h"
#include "core/support/font/font_style.h"
#include "core/support/font/typeface.h"
#include "core/support/font/types.h"
#include "core/support/text/text_blob.h"

namespace tdfcore {

class TextShaper : public Object {
 public:
  // 文字方向，如果开启unicode icu需要更加bidi level来判断
  enum Direction {
    kLTR,
    kRTL,
  };

  // 创建一个基础版的TextShaper，不走HarfBuzz
  static std::unique_ptr<TextShaper> MakePrimitive();

#ifdef TEXT_SHAPER_HARFBUZZ_AVAILABLE
  static std::unique_ptr<TextShaper> MakeShapeDontWrapOrReorder(std::shared_ptr<TFontManager> = nullptr);
  static void PurgeHarfBuzzCache();
#endif
#ifdef TEXT_SHAPER_CORETEXT_AVAILABLE
  static std::unique_ptr<TextShaper> MakeCoreText();
#endif
  static std::unique_ptr<TextShaper> Make(std::shared_ptr<TFontManager> = nullptr);
  static void PurgeCaches();

  TextShaper();
  virtual ~TextShaper();

  // 迭代器基类，在遍历Unicode的过程中需要跑不同的迭代器来生成相应的Run信息
  class RunIterator {
   public:
    virtual ~RunIterator() = default;
    // 迭代器运行到末尾
    virtual void Consume() = 0;
    // 当前运行中最后一个 (utf8) 元素的偏移量
    virtual size_t EndOfCurrentRun() const = 0;
    // 如果不再调用消耗，则返回 true
    virtual bool AtEnd() const = 0;
  };

  // 字体迭代器
  class FontRunIterator : public RunIterator {
   public:
    virtual const std::shared_ptr<TFont>& CurrentFont() const = 0;
  };
  // 文字方向迭代器
  class BiDiRunIterator : public RunIterator {
   public:
    // 当前文字方向
    virtual Direction CurrentDirection() const = 0;
  };
  // iso编码迭代器
  class ScriptRunIterator : public RunIterator {
   public:
    // iso15924 codes
    virtual FourByteTag CurrentScript() const = 0;
  };
  // 语言迭代器
  class LanguageRunIterator : public RunIterator {
   public:
    // 当前语言，"zh" "ch" ...
    virtual const char* CurrentLanguage() const = 0;
  };

  struct Feature {
    FourByteTag tag;
    uint32_t value;
    size_t start;
    size_t end;
  };

 private:
  template <typename RunIteratorSubclass>
  class TrivialRunIterator : public RunIteratorSubclass {
   public:
    static_assert(std::is_base_of<RunIterator, RunIteratorSubclass>::value, "");
    explicit TrivialRunIterator(size_t u8_bytes) : end_(u8_bytes), at_end_(end_ == 0) {}
    void Consume() override { at_end_ = true; }
    size_t EndOfCurrentRun() const override { return at_end_ ? end_ : 0; }
    bool AtEnd() const override { return at_end_; }

   private:
    size_t end_;
    bool at_end_;
  };

 public:
  static std::unique_ptr<FontRunIterator> MakeFontMgrRunIterator(const char* utf8, size_t u8_bytes,
                                                                 const std::shared_ptr<TFont>& font,
                                                                 std::shared_ptr<TFontManager> fallback);
  static std::unique_ptr<FontRunIterator> MakeFontMgrRunIterator(const char* utf8, size_t u8_bytes,
                                                                 const std::shared_ptr<TFont>& font,
                                                                 std::shared_ptr<TFontManager> fallback,
                                                                 const char* request_name, TFontStyle request_style,
                                                                 const TextShaper::LanguageRunIterator*);

  class TrivialFontRunIterator : public TrivialRunIterator<FontRunIterator> {
   public:
    TrivialFontRunIterator(const std::shared_ptr<TFont>& font, size_t u8_bytes)
        : TrivialRunIterator(u8_bytes), font_(font) {}
    const std::shared_ptr<TFont>& CurrentFont() const override { return font_; }

   private:
    std::shared_ptr<TFont> font_;
  };

  static std::unique_ptr<BiDiRunIterator> MakeBiDiRunIterator(const char* utf8, size_t u8_bytes, Direction direction);

#if defined(TEXT_SHAPER_HARFBUZZ_AVAILABLE)
  static std::unique_ptr<BiDiRunIterator> MakeUnicodeBidiRunIterator(const char* utf8, size_t u8_bytes);
  static std::unique_ptr<BiDiRunIterator> MakeIcuBiDiRunIterator(const char* utf8, size_t u8_bytes,
                                                                 Direction direction);
#endif

  class TrivialBiDiRunIterator : public TrivialRunIterator<BiDiRunIterator> {
   public:
    TrivialBiDiRunIterator(Direction direction, size_t u8_bytes)
        : TrivialRunIterator(u8_bytes), direction_(direction) {}
    // 这里正确的做法是返回BidiLevel，然后根据BidiLevel来判断方向，但目前没有依赖icu所以无法做多语言方向判断
    Direction CurrentDirection() const override { return direction_; }

   private:
    Direction direction_ = Direction::kLTR;
  };

  static std::unique_ptr<ScriptRunIterator> MakeScriptRunIterator(const char* utf8, size_t u8_bytes,
                                                                  FourByteTag script);
#if defined(TEXT_SHAPER_HARFBUZZ_AVAILABLE)
  static std::unique_ptr<ScriptRunIterator> MakeUnicodeHbScriptRunIterator(const char* utf8, size_t u8_bytes);
  static std::unique_ptr<ScriptRunIterator> MakeHbIcuScriptRunIterator(const char* utf8, size_t u8_bytes);
#endif
  class TrivialScriptRunIterator : public TrivialRunIterator<ScriptRunIterator> {
   public:
    TrivialScriptRunIterator(FourByteTag script, size_t utf8Bytes) : TrivialRunIterator(utf8Bytes), script_(script) {}
    FourByteTag CurrentScript() const override { return script_; }

   private:
    FourByteTag script_;
  };

  static std::unique_ptr<LanguageRunIterator> MakeStdLanguageRunIterator(const char* utf8, size_t u8_bytes);
  class TrivialLanguageRunIterator : public TrivialRunIterator<LanguageRunIterator> {
   public:
    TrivialLanguageRunIterator(const char* language, size_t utf8Bytes)
        : TrivialRunIterator(utf8Bytes), language_(language) {}
    const char* CurrentLanguage() const override { return language_.c_str(); }

   private:
    std::string language_;
  };

  class RunHandler {
   public:
    virtual ~RunHandler() = default;

    struct Range {
      constexpr Range() : begin(0), size(0) {}
      constexpr Range(size_t begin, size_t size) : begin(begin), size(size) {}
      size_t begin;
      size_t size;
      constexpr size_t end() const { return begin + size; }
    };

    struct Info {
      std::shared_ptr<TFont> font;
      Direction direction;
      Vector advance;
      std::vector<TPoint> advances;
      size_t glyph_count;
      Range u8_range;
    };

    struct Buffer {
      GlyphID* glyphs;     // 字形数据
      TPoint* positions;   // 位置信息，最终字形位置信息 = positions[i] + offsets[i]
      TPoint* offsets;     // 字形偏移信息
      uint32_t* clusters;  // utf8+clusters[i] 得到字形 glyphs[i]
      TPoint point;        // 所有字形position的偏移
    };

    //  当开始一行时会触发此回调
    virtual void BeginLine() = 0;

    // 每一行每次运行都会调用一次，用来计算baseline和偏移信息
    virtual void RunInfo(const Info&) = 0;

    // 在每一行RunInfo调用结束是调用此方法
    virtual void CommitRunInfo() = 0;

    // 在CommitRunInfo之后的每一行调用此方法来填充Buffer信息
    virtual Buffer RunBuffer(const Info&) = 0;

    // 在RunBuffer调用结束调用此方法
    virtual void CommitRunBuffer(const Info&) = 0;

    // 每一行运行结束时调用此方法
    virtual void CommitLine() = 0;
  };

  virtual void Shape(const char* utf8, size_t u8_bytes, const std::shared_ptr<TFont>& font, bool left_to_right,
                     TScalar width, RunHandler*) const = 0;

  virtual void Shape(const char* utf8, size_t u8_bytes, FontRunIterator&, BiDiRunIterator&, ScriptRunIterator&,
                     LanguageRunIterator&, TScalar width, RunHandler*) const = 0;

  virtual void Shape(const char* utf8, size_t u8_bytes, FontRunIterator&, BiDiRunIterator&, ScriptRunIterator&,
                     LanguageRunIterator&, const Feature* features, size_t features_size, TScalar width,
                     RunHandler*) const = 0;

 private:
  TextShaper(const TextShaper&) = delete;
  TextShaper& operator=(const TextShaper&) = delete;
};

/***
 * @brief 给TextShaper在shaper过程创建TextBlob
 */
class TextBlobBuilderRunHandler final : public TextShaper::RunHandler {
 public:
  explicit TextBlobBuilderRunHandler(TPoint offset) : offset_(offset) {}
  std::vector<std::shared_ptr<TextBlob>> GetTextBlobs();
  TPoint EndPoint() { return offset_; }

  void BeginLine() override;
  void RunInfo(const Info&) override;
  void CommitRunInfo() override;
  Buffer RunBuffer(const Info&) override;
  void CommitRunBuffer(const Info&) override;
  void CommitLine() override;

 private:
  size_t cluster_offset_;
  size_t glyph_count_;
  TScalar max_run_ascent_;
  TScalar max_run_descent_;
  TScalar max_run_leading_;
  TPoint current_position_;
  TPoint offset_;
  std::vector<uint32_t> clusters_;
  std::vector<GlyphID> glyphs_;
  std::vector<TPoint> positions_;
  std::shared_ptr<TFont> current_font_;
  std::vector<std::shared_ptr<TextBlob>> text_blobs_;
};

}  // namespace tdfcore
