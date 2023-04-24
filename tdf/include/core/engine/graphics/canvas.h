//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include "core/common/color.h"
#include "core/common/impl_getter.h"
#include "core/common/m33.h"
#include "core/common/m44.h"
#include "core/common/object.h"
#include "core/common/rrect.h"
#include "core/common/scalar.h"
#include "core/engine/graphics/texture.h"
#include "core/engine/graphics/image_filter.h"
#include "core/engine/graphics/paint.h"
#include "core/engine/graphics/path.h"
#include "core/engine/graphics/picture.h"
#include "core/engine/graphics/save_layer_rec.h"
#include "core/support/text/text_blob.h"

namespace tdfcore {

class Canvas : public Object {
 public:
  /**
   * @brief 获取宽度
   * @return 宽度
   */
  virtual int GetWidth() const = 0;

  /**
   * @brief 获取高度
   * @return 高度
   */
  virtual int GetHeight() const = 0;

  virtual int save() = 0;

  virtual int saveLayer(const TRect &bounds, const TPaint *paint) = 0;

  virtual void restoreLayer() = 0;

  virtual int saveLayer(const SaveLayerRec &layerRec) = 0;

  virtual void restore() = 0;

  virtual int getSaveCount() const = 0;

  virtual void restoreToCount(int saveCount) = 0;

  virtual void clear(Color color) = 0;

  virtual ISize getBaseLayerSize() const = 0;

  virtual void translate(TScalar dx, TScalar dy) = 0;

  virtual void rotate(TScalar degree) = 0;

  virtual void rotate(TScalar degree, TScalar px, TScalar py) = 0;

  virtual void scale(TScalar sx, TScalar sy) = 0;

  virtual void concat(const TM44 &matrix) = 0;

  virtual void setMatrix(const TM44 &matrix) = 0;

  virtual void resetMatrix() = 0;

  virtual TM44 getTotalMatrix() const = 0;

  virtual void clipRect(const TRect &rect) = 0;

  virtual void clipRRect(const RRect &rrect) = 0;

  virtual void clipPath(const Path &path) = 0;

  virtual void drawArc(const TRect& oval, TScalar startAngle, TScalar sweepAngle, bool useCenter,
                       const TPaint& paint) = 0;

  virtual void drawPaint(const TPaint& paint) = 0;

  virtual void drawLine(TScalar x0, TScalar y0, TScalar x1, TScalar y1, const TPaint &paint) = 0;

  virtual void drawRect(const TRect &rect, const TPaint &paint) = 0;

  virtual void drawRRect(const RRect &rrect, const TPaint &paint) = 0;

  virtual void drawCircle(TScalar cx, TScalar cy, TScalar radius, const TPaint &paint) = 0;

  virtual void drawPath(const Path &path, const TPaint &paint) = 0;

  virtual void drawImage(const std::shared_ptr<Texture> &, TScalar x, TScalar y, const TPaint *paint = nullptr) = 0;

  virtual void drawImageRect(const std::shared_ptr<Texture> &, const TRect &src, const TRect &dst,
                             const TPaint *paint = nullptr) = 0;
  virtual void drawImageRect(const std::shared_ptr<Texture> &, const TRect &dst, const TPaint *paint = nullptr) = 0;

  virtual void drawTextBlob(const std::shared_ptr<TextBlob> &blob, TScalar x, TScalar y, const TPaint &paint) = 0;

  virtual void drawGlyphs(const std::vector<GlyphID> &glyph_ids, const std::vector<TPoint> &positions, TScalar x,
                          TScalar y, const std::shared_ptr<TFont> &font, const TPaint &paint) = 0;

  virtual void drawAtlas(const std::shared_ptr<Texture> &atlas, const std::vector<TM33> &matrixs,
                         const std::vector<TRect> &rects, const std::vector<Color> &colors) = 0;

  virtual void drawSimpleText(const std::u16string& text, TScalar x, TScalar y, const std::shared_ptr<TFont>& font,
                              const TPaint& paint) = 0;

  virtual void drawPicture(const std::shared_ptr<Picture> &picture, const TM33 *matrix = nullptr,
                           const TPaint *paint = nullptr) = 0;
  virtual void flush() = 0;

  static std::shared_ptr<Canvas> MakeMock();
};

}  // namespace tdfcore
