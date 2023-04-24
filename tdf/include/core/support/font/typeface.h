//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include "core/common/data.h"
#include "core/common/impl_getter.h"
#include "core/common/object.h"
#include "core/support/font/font_style.h"
#include "core/support/font/types.h"
#if TDF_PLATFORM_MACOS
#  import <ApplicationServices/ApplicationServices.h>
#endif
#if TDF_PLATFORM_IOS
#  include <CoreText/CoreText.h>
#endif
namespace tdfcore {
class TTypeface : public Object, public ImplGetter {
 public:
/****
 * @brief
 * impl具体实现类参数，在skia环境下必须是std::shared_ptr<SkTypeface>对象，在tgfx环境下必须是std::shared_ptr<tgfx::Typeface>对象
 * GetImpl的cast类型也与之对应
 * @param impl
 */
  static std::shared_ptr<TTypeface> Make(std::any impl);

  static std::shared_ptr<TTypeface> Default();

  static bool Equal(const std::shared_ptr<TTypeface>& a, std::shared_ptr<TTypeface>& b);

  virtual TFontStyle GetFontStyle() const = 0;

  virtual TTypefaceID GetUniqueID() const = 0;

  virtual bool HasColor() const = 0;

  virtual int TextToGlyphs(const void* text, size_t byte_length, TTextEncoding encoding, GlyphID glyphs[],
               int max_glyph_count) const = 0;

  virtual GlyphID GetGlyphID(const std::string& name) = 0;

  virtual GlyphID UnicharToGlyph(Unichar unichar) const = 0;

  virtual void UnicharsToGlyphs(const std::vector<Unichar>& uni_chars, GlyphID glyphs[]) const = 0;

  virtual int GetUnitsPerEm() const = 0;

  virtual std::shared_ptr<TData> OpenExistingStream(int* ttcIndex) const = 0;

  virtual std::shared_ptr<TData> CopyTableData(TFontTableTag tag) const = 0;

  virtual std::string GetFontFamily() const = 0;

 protected:
  FRIEND_OF_TDF_ALLOC;
};
#if (defined TARGET_OS_MAC) || (defined TARGET_OS_IOS)
std::shared_ptr<TTypeface> MakeTypefaceFromCTFont(CTFontRef);
CTFontRef TypefaceGetCTFontRef(const std::shared_ptr<TTypeface>& face);
#endif

}  // namespace tdfcore
