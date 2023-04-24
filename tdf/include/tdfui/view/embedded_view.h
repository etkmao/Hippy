//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/id_generator.h"
#include "core/engine/render/embedded_object.h"
#include "core/support/gesture/recognizer/pan_gesture_recognizer.h"
#include "core/support/gesture/recognizer/scroll_gesture_recognizer.h"
#include "core/support/gesture/recognizer/tap_gesture_recognizer.h"
#include "core/support/gesture/recognizer/long_press_gesture_recognizer.h"
#include "core/support/gesture/recognizer/double_tap_gesture_recognizer.h"
#include "tdfui/view/view.h"
#include "tdfui/view/view_context.h"

namespace tdfcore {
class EmbeddedView : public View {
  TDF_REFF_CLASS_META(EmbeddedView)

 public:
  /**
   * @brief 设置属性
   * @param property 属性
   */
  void SetProperty(const std::map<std::string, std::string> &property);

  /**
   * @brief 获取属性
   * @return 属性
   */
  constexpr const std::map<std::string, std::string> &GetProperty() const { return property_; }

  /**
   * @brief 设置 EmbeddedView 节点对应的参与HitTest的区域，可空（此时使用系统的）
   */
  void SetHitTestRect(std::optional<TRect> rect);

  /**
   * @brief 设置 EmbeddedView 节点支持的手势类型，默认全支持，不需要主动调用
   */
  void SetSupportedGestures(const std::set<Type> &supported);

 protected:
  enum RenderObjectIndex { kEmbedded = View::kRenderObjectCount, kRenderObjectCount };

  explicit EmbeddedView(const std::string &view_type, uint64_t id = IDGenerator::NextUniqueID(),
                        const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent());

  void Init() override;

  int32_t GetTotalRenderObjectCount() override;

 private:
  std::string view_type_;
  uint64_t embedded_id_;
  std::map<std::string, std::string> property_;
  std::shared_ptr<TapGestureRecognizer> tap_recognizer_;
  std::shared_ptr<ScrollGestureRecognizer> scroll_recognizer_;
  std::shared_ptr<LongPressGestureRecognizer> long_press_recognizer_;
  std::shared_ptr<DoubleTapGestureRecognizer> double_tap_recognizer_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::EmbeddedView, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::EmbeddedView)
