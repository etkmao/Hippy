//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/raster/layers/layer_link.h"
#include "core/engine/render/editable_object.h"
#include "core/engine/render/follower_object.h"
#include "core/support/text/text_base.h"
#include "tdfui/view/clipboard_status_notifier.h"
#include "core/platform/common/text_input.h"
#include "tdfui/view/text/text_selection_control.h"
#include "tdfui/view/view.h"

namespace tdfcore {
class TextInputToolbarView : public View {
  TDF_REFF_CLASS_META(TextInputToolbarView)

 public:
  void SetSelection(const TextSelection &selection);
  void SetClipboardStatusNotifier(std::shared_ptr<ClipboardStatusNotifier> notifier);
  void UpdatePosition();

 protected:
  enum RenderObjectIndex { kFollower = kRenderObjectCount, kRenderObjectCount };

  explicit TextInputToolbarView(std::shared_ptr<LayerLink> link,
                                std::weak_ptr<EditableObject> editable, TextSelection selection,
                                std::shared_ptr<TextSelectionControl> selection_control,
                                std::weak_ptr<TextSelectionDelegate> selection_delegate,
                                const std::shared_ptr<ViewContext>& view_context = ViewContext::GetCurrent());

  void Init() override;

  int32_t GetTotalRenderObjectCount() override;

 private:
  TRect GetEditingRegion();
  static constexpr size_t kToolbarVerticalGap = 16;
  std::shared_ptr<FollowerObject> follower_;
  std::shared_ptr<LayerLink> layer_link_;
  std::shared_ptr<TextSelectionControl> selection_control_;
  std::weak_ptr<TextSelectionDelegate> selection_delegate_;
  std::shared_ptr<ClipboardStatusNotifier> clipboard_status_;
  TextSelection selection_;
  std::weak_ptr<EditableObject> editable_;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::TextInputToolbarView, bases<tdfcore::View>)
TDF_REFL_END(tdfcore::TextInputToolbarView)
