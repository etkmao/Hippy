//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#include "core/engine/raster/layers/layer_link.h"
#include "core/engine/render/editable_object.h"
#include "core/engine/render/follower_object.h"
#include "tdfui/view/text/text_selection_control.h"
#include "tdfui/view/view.h"
#include "core/support/gesture/recognizer/drag_detail.h"

#pragma once
namespace tdfcore {
class TextSelectionHandlerView : public View {
  TDF_REFF_CLASS_META(TextSelectionHandlerView)

 public:
  using SelectionHandlerChangedCallback = std::function<void(const TextSelection &)>;
  using SelectionHandlerTappedCallback = std::function<void()>;

  void NotifySelectionHandleTapped();
  void NotifySelectionHandleChanged(const TextSelection &selection);
  void SetOnSelectionHandlerChangedCallback(SelectionHandlerChangedCallback callback);
  void SetOnSelectionHandlerTappedCallback(SelectionHandlerTappedCallback callback);
  void SetSelection(const TextSelection &selection);

 protected:
  enum RenderObjectIndex { kHandle = kRenderObjectCount, kRenderObjectCount };
  TextSelectionHandlerView(const TextSelectionHandlePosition &position, const std::shared_ptr<LayerLink> &link,
                           const std::weak_ptr<EditableObject> &editable, const TextSelection &selection,
                           const std::shared_ptr<TextSelectionControl> &control,
                           const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent());
  void Init() override;
  int32_t GetTotalRenderObjectCount() override;
  std::shared_ptr<RenderObject> GetRootRenderObject() const override;
  void Mount() override;
  void Unmount() override;

 private:
  void AddGestureEvents(const std::shared_ptr<RenderObject> &handler);
  void HandleDragUpdate(const DragUpdateDetails &details);
  void HandleDragStart(const DragDownDetails &details);
  void OnVisibilityChanged(bool visibility);
  TextSelection MakeSelection(TextPosition new_position);
  TextSelectionHandleType GetHandleType();

  TRect interactive_rect_;
  TPoint drag_position_ = TPoint::Make(0, 0);
  TextSelectionHandlePosition position_;
  std::shared_ptr<FollowerObject> follower_;
  std::shared_ptr<LayerLink> layer_link_;
  std::shared_ptr<TextSelectionControl> selection_control_;
  TextSelection selection_;
  std::weak_ptr<EditableObject> editable_;
  SelectionHandlerChangedCallback on_selection_handle_changed_;
  SelectionHandlerTappedCallback on_selection_handle_tapped_;
  TPoint handle_offset_;
  TSize handle_size_;
  ValueNotifier<bool>::Callback visibility_change_listener_;
  uint64_t visibility_change_listener_handler_ = 0;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::TextSelectionHandlerView, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::TextSelectionHandlerView)
