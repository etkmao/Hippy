//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include "core/common/object.h"
#include "core/support/font/font.h"
#include "core/support/font/font_style.h"
#include "core/support/font/typeface.h"

namespace tdfcore {

class TFontManager : public Object, public ImplGetter {
 public:
  /****
   * @brief
   * impl具体实现类参数，在skia环境下必须是sk_sp<SkFontMgr>对象，在tgfx环境下必须是std::shared_ptr<FontManager>对象
   * GetImpl的cast类型也与之对应
   * @param impl
   */
  static std::shared_ptr<TFontManager> Make(std::any impl);

  static std::shared_ptr<TFontManager> Default();

  virtual std::shared_ptr<TTypeface> MakeFromFile(const std::string& path, int ttc_index = 0) = 0;

  virtual std::shared_ptr<TTypeface> MakeFromData(const void* data, size_t length, int ttc_index = 0) = 0;

  virtual std::shared_ptr<TTypeface> GetTypeface(const std::string& family,
                                                 const TFontStyle& font_style = TFontStyle()) = 0;

  virtual std::shared_ptr<TTypeface> GetFallbackTypeface(Unichar unichar, std::string_view locale = "") = 0;

  virtual std::shared_ptr<TTypeface> GetFallbackTypeface(const std::string& name, GlyphID& id,
                                                         std::string_view locale = "") = 0;

  virtual std::vector<std::string> GetFamilies() = 0;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
