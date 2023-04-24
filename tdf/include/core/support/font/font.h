//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include "core/common/rect.h"
#include "core/common/scalar.h"
#include "core/engine/graphics/paint.h"
#include "core/support/font/typeface.h"
#include "core/support/font/types.h"

namespace tdfcore {

class TFont : public Object, public ImplGetter {
 public:
  static std::shared_ptr<TFont> Make();

  static std::shared_ptr<TFont> Make(const std::shared_ptr<TTypeface>& typeface, TScalar size = 12);

  virtual std::shared_ptr<TFont> Clone() = 0;
  /****
   * @brief
   * impl具体实现类参数，在skia环境下必须是std::shared_ptr<SkFont>对象，
   * 在tgfx环境下必须是std::shared_ptr<tgfx::Font>对象。
   * GetImpl的cast类型也与之对应。
   * 这只是一个过渡方案，一般不推荐直接用SetImpl/GetImpl 。
   */
  virtual void SetImpl(std::any impl) = 0;

  virtual void SetSize(TScalar size) = 0;

  virtual void SetFakeBold(bool fake_bold) = 0;

  virtual void SetFakeItalic(bool fake_italic) = 0;

  virtual void SetScaleX(TScalar scale) = 0;

  virtual void SetSubpixel(bool subpixel) = 0;

  virtual void SetEdging(Edging edging) = 0;

  virtual void SetHinting(TFontHinting hinting_level) = 0;

  virtual void SetTypeface(const std::shared_ptr<TTypeface>& typeface) = 0;

  virtual TScalar MeasureText(const void* text, size_t length, TTextEncoding encoding, TRect& bounds,
                              const std::shared_ptr<TPaint>& paint = nullptr) const  = 0;

  virtual bool IsBold() const = 0;

  virtual bool IsItalic() const  = 0;

  virtual float GetGlyphAdvance(GlyphID id) const = 0;

  virtual TRect GetGlyphBounds(GlyphID id) const = 0;

  virtual TScalar GetSize() const = 0;

  virtual TScalar GetScaleX() const = 0;

  virtual TFontMetrics GetMetrics() const = 0;

  virtual bool IsSubpixel() const = 0;

  virtual void GetWidthsBounds(const GlyphID glyphs[], int count,
                               TScalar widths[], TRect bounds[], const TPaint* paint) const = 0;

  virtual int CountText(const void *text, size_t byteLength, TTextEncoding encoding) const = 0;

  virtual bool operator==(const TFont &font) = 0;

  virtual void GlyphsToUnichars(const std::vector<GlyphID> glyphs, Unichar unichars[]) = 0;

  virtual void GetPaths(const GlyphID glyphs[], int count,
                        void (*path_proc)(const Path* path, const TM33& matrix, void* ctx), void* ctx) = 0;

  std::shared_ptr<TTypeface> GetTypeface() const { return typeface_; }

  std::shared_ptr<TTypeface> GetTypefaceOrDefault() const {
    if (typeface_) {
      return typeface_;
    }
    return TTypeface::Default();
  }

  void GetWidths(const GlyphID glyphs[], int count, TScalar widths[]) const {
    this->GetWidthsBounds(glyphs, count, widths, nullptr, nullptr);
  }

  void GetWidths(const GlyphID glyphs[], int count, TScalar widths[], TRect bounds[]) const {
    this->GetWidthsBounds(glyphs, count, widths, bounds, nullptr);
  }

  void GetBounds(const GlyphID glyphs[], int count, TRect bounds[], const TPaint* paint) const {
    this->GetWidthsBounds(glyphs, count, nullptr, bounds, paint);
  }

  GlyphID UnicharToGlyph(Unichar unichar) { return this->GetTypefaceOrDefault()->UnicharToGlyph(unichar); }

  int TextToGlyphs(const void* text, size_t byteLength, TTextEncoding encoding,
                   GlyphID glyphs[], int maxGlyphCount) const {
    return this->GetTypefaceOrDefault()->TextToGlyphs(text, byteLength, encoding, glyphs, maxGlyphCount);
  }

 protected:
  std::shared_ptr<TTypeface> typeface_;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
