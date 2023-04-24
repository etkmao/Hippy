//
// Copyright © 2020 com.tencent. All rights reserved.
//

#pragma once

#include <vector>
#include "core/common/object.h"
#include "core/common/point.h"
#include "core/common/rect.h"
#include "core/support/font/font.h"

namespace tdfcore {

class TextBlob : public Object, public ImplGetter {
 public:
  static std::shared_ptr<TextBlob> MakeFromText(const std::u16string& text, const std::shared_ptr<TFont>& font);

  TRect GetBounds() const { return bounds_; }

  void SetBounds(const TRect& bounds) { bounds_ = bounds; }

  std::vector<GlyphID> GetGlyphIDs() const { return glyph_ids_; }
  std::vector<TPoint> GetPositions() const { return positions_; }
  std::shared_ptr<TFont> GetFont() const { return font_; }

  static std::shared_ptr<TextBlob> Make(const std::vector<GlyphID>& ids,
                                        const std::vector<TPoint>& positions,
                                        const std::shared_ptr<TFont>& font);
  static std::shared_ptr<TextBlob> Make(const std::vector<GlyphID>& ids,
                                        const std::shared_ptr<TFont>& font);
  /****
   * @brief
   * impl具体实现类参数，在skia环境下必须是std::shared_ptr<SkTextBlob>对象，在tgfx环境没有提供相应的转换
   * GetImpl的cast类型也与之对应
   * @param impl
   */
  static std::shared_ptr<TextBlob> Make(std::any impl);

 protected:
  std::vector<GlyphID> glyph_ids_;
  std::vector<TPoint> positions_;
  std::shared_ptr<TFont> font_;
  TRect bounds_;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
