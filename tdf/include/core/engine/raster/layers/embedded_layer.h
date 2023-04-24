//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <string>

#include "core/common/rect.h"
#include "core/engine/graphics/texture.h"
#include "core/engine/raster/layers/layer.h"
#include "core/engine/render/render_property.h"

namespace tdfcore {
class LayerTreeBuilder;

/**
 * @brief       表示PlatformView的Layer
 */
class EmbeddedLayer : public Layer {
  TDF_REFF_CLASS_META(EmbeddedLayer)

 public:
  void AddToLayerTree(std::shared_ptr<LayerTreeBuilder> builder, const TPoint &offset = TPoint::Make(0, 0)) override;

  void Preroll(PrerollContext &context, const TM33 &matrix) override;

  void Paint(PaintContext &context) const override;

  /**
   * @brief     设置的显示区域（本地坐标系）
   * @param     rect
   */
  void SetRect(const TRect &rect);

  /**
   * @brief     获取显示区域
   * @return    区域
   */
  TRect GetRect() const;

  /**
   * @brief     设置编号
   * @param     id 编号
   */
  void SetEmbeddedId(uint64_t id);
  /**
   * @brief 获取Embedded ID
   */
  constexpr uint64_t GetEmbeddedId() const { return embedded_id_; }
  void SetType(std::string view_type);
  std::string GetType() { return type_; }
  void SetProperty(const std::map<std::string, std::string> &property);
  void SetSnapshot(const std::shared_ptr<Texture> &snapshot);
  std::string ToString() const override;

 protected:
  EmbeddedLayer(const TRect &rect, uint64_t embedded_id, std::string type,
                const std::map<std::string, std::string> &property, uint64_t id = IDGenerator::NextUniqueID());

 private:
  TRect rect_;
  uint64_t embedded_id_;
  std::string type_;
  std::map<std::string, std::string> prop_;
  std::shared_ptr<Texture> snapshot_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::EmbeddedLayer, bases<tdfcore::Layer>)
TDF_REFL_MEMBER(rect_);
TDF_REFL_MEMBER(embedded_id_);
TDF_REFL_MEMBER(type_);
TDF_REFL_END(tdfcore::EmbeddedLayer)
