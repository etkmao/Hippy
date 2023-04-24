//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include <vector>
#include "tdfui/view/clipboard_status_notifier.h"
#include "tdfui/view/text/text_selection_control.h"
#include "tdfui/view/view.h"

namespace tdfcore {
class LayerLink;
class AnimationController;
class EditableObject;

class TextSelectionOverlay : public Object, public std::enable_shared_from_this<TextSelectionOverlay> {
 public:
  static constexpr TDuration kFadeDuration = std::chrono::milliseconds(150);

  ~TextSelectionOverlay() override = default;
  void ShowSelectionHandles();
  void HideSelectionHandles();
  void ShowToolbar();
  void HideToolbar();
  void Hide();
  void Update(TextEditingValue value);
  void UpdateSelectionHandles();
  void UpdateToolbar();
  void Dispose();
  void SetEditingValue(const TextEditingValue &value);
  constexpr const std::shared_ptr<TextSelectionControl> &GetSelectionControl() const { return selection_control_; }
  void UpdateViewportOffset(const TPoint &offset);

 protected:
  TextSelectionOverlay(TextEditingValue value, std::shared_ptr<EditableObject> editable,
                       std::shared_ptr<LayerLink> toolbar_layer_link,
                       std::shared_ptr<LayerLink> start_handle_layer_link,
                       std::shared_ptr<LayerLink> end_handle_layer_link,
                       std::shared_ptr<TextSelectionControl> selection_control = nullptr,
                       std::shared_ptr<TextSelectionDelegate> selection_delegate = nullptr,
                       std::function<void()> selection_handle_tapped = nullptr,
                       std::shared_ptr<ClipboardStatusNotifier> clipboard_status = nullptr);
  std::shared_ptr<View> BuildHandle(TextSelectionHandlePosition position);
  std::shared_ptr<View> BuildToolbar();
  void HandleSelectionHandleChanged(const tdfcore::TextSelection &selection,
                                    TextSelectionHandlePosition handle_position);

 private:
  std::shared_ptr<AnimationController> CreateToolbarAnimationController(double start_value, double to_value);

  TextEditingValue value_;
  std::shared_ptr<EditableObject> editable_;
  std::shared_ptr<LayerLink> toolbar_layer_link_ = nullptr;
  std::shared_ptr<LayerLink> start_handle_layer_link_ = nullptr;
  std::shared_ptr<LayerLink> end_handle_layer_link_ = nullptr;
  std::shared_ptr<TextSelectionControl> selection_control_;
  std::weak_ptr<TextSelectionDelegate> selection_delegate_;
  std::function<void()> selection_handle_tapped_;
  std::shared_ptr<ClipboardStatusNotifier> clipboard_status_;
  bool is_selection_handle_shown_ = false;
  bool is_toolbar_shown_ = false;
  std::vector<std::shared_ptr<View>> selection_handles_;
  std::shared_ptr<View> toolbar_;
  std::shared_ptr<AnimationController> toolbar_controller_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
