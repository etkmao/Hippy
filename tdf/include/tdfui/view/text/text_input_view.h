//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/engine/render/editable_object.h"
#include "core/engine/render/offstage_object.h"
#include "core/platform/common/keyboard/keyboard.h"
#include "core/support/animation/animation_controller.h"
#include "core/support/gesture/recognizer/double_tap_gesture_recognizer.h"
#include "core/support/gesture/recognizer/long_press_gesture_recognizer.h"
#include "core/support/gesture/recognizer/tap_gesture_recognizer.h"
#include "core/support/gesture/recognizer/hover_gesture_recognizer.h"
#include "tdfui/view/scroll_view.h"
#include "tdfui/view/text/text_editing_controller.h"
#include "tdfui/view/text/text_selection_control.h"
#include "tdfui/view/view.h"

namespace tdfcore {
class ATimer;
class AnimationController;

class TextSelectionOverlay;

class TextInputView : public ScrollView, public TextSelectionDelegate {
  TDF_REFF_CLASS_META(TextInputView)

 public:
  // 编辑行为结束时触发，携带参数：编辑结束时文本框的内容
  using EndEditingCallback = std::function<void(const std::u16string &)>;
  // 属性集合
  struct Attributes {
   public:
    using OnSelectionChanged =
        std::function<void(const TextSelection &text_selection, EditableObject::SelectionChangeCause cause)>;

    ParagraphStyle paragraph_style;
    bool is_editable = true;
    float prefer_height = 0;
    size_t max_length = std::numeric_limits<size_t>::max();
    std::shared_ptr<TextSelectionControl> selection_control = nullptr;
    std::shared_ptr<TextEditingController> editing_controller = nullptr;
    std::u16string placeholder;
    Color placeholder_color;
    OnSelectionChanged on_selection_changed = nullptr;
    std::function<void()> on_selection_handle_tapped = nullptr;
    std::function<void()> on_editing_complete = nullptr;
    std::function<void(const std::u16string &)> on_submitted = nullptr;
    std::optional<KeyboardInputType> input_type;
    KeyboardAction keyboard_action;
    bool obscure_text = true;
    bool auto_correct = true;
    SmartDashesType smart_dashes_type = SmartDashesType::kDashesNone;
    SmartQuotesType smart_quotes_type = SmartQuotesType::kQuotesNone;
    bool enable_suggestions;
    std::optional<std::string> action_label;
    TextCapitalization text_capitalization = TextCapitalization::kWords;
    Brightness brightness = Brightness::kLight;
  };

  const Attributes &GetAttributes() const { return attributes_; }
  void SetTextStyle(const TextStyle &style);
  void SetEditable(bool is_editable);
  void SetInputType(KeyboardInputType type);
  void SetKeyboardAction(KeyboardAction action);
  void SetText(const std::u16string &text);
  void SetPlaceholder(const std::u16string &placeholder, const Color &color);
  void SetEndEditingCallback(EndEditingCallback callback);
  void SetMaxLength(size_t length);
  void SetMaxLines(size_t count);
  void SetHorizontalAlign(HorizontalAlign align);
  void SetVerticalAlign(VerticalAlign align);
  void SetEditingController(std::shared_ptr<TextEditingController> editing_controller);
  void SetSelectionControl(std::shared_ptr<TextSelectionControl> selection_control);
  /**
   * 设置行高倍率
   * @param height
   */
  void SetLineHeightMultiplier(float height);

  void SetCursorWidth(TScalar width);
  TScalar GetCursorWidth() const { return editable_attributes_.cursor_width; }
  void SetCursorRadius(TScalar radius);
  TScalar GetCursorRadius() const { return editable_attributes_.cursor_radius; }
  void SetCursorColor(const Color &color);
  Color GetCursorColor() const { return editable_attributes_.cursor_color; }
  void SetSelectionColor(const Color &color);
  Color GetSelectionColor() const { return editable_attributes_.selection_color; }
  void SetIsShowCursor(bool is_show);
  bool GetIsShowCursor() const { return editable_attributes_.show_cursor; }

  /**
   * 获取输入框滚动方向
   * @return
   */
  TextAxis GetAxis() const;

  /**
   * 尝试隐藏/显示软键盘（仅当此TextInputView持有焦点时生效）
   */
  void TryHidingInputMethod();
  void TryShowingInputMethod();

  /**
   * 设置Text变化回调
   * 仅回调非合成(Composing)文字变化
   * 拼音输入法输入时，会有合成态，这部分合成态将不会回调，当中文完成输入时才会回调
   * 英文输入正常回调
   */
  void SetTextDidChangeCallback(const std::function<void(const std::vector<std::shared_ptr<InlineSpan>>&)> &callback) {
    text_did_change_callback_ = callback;
  }

  /**
   * 获取Text变化回调
   */
  std::function<void(const std::vector<std::shared_ptr<InlineSpan>>&)> GetTextDidChangeCallback() const {
    return text_did_change_callback_;
  }

  /**
   * @ 设置选区变化回调
   * @param seleciton_changed
   */
  void SetTextSelectionChangeCallback(const Attributes::OnSelectionChanged &seleciton_changed) {
    attributes_.on_selection_changed = seleciton_changed;
  }

  // TextSelectionDelegate
  const TextEditingValue &GetTextEditingValue() override;
  void UserUpdateTextEditingValue(const class TextEditingValue &value) override;
  void HideSelectionHandles();
  void BringIntoView(TextPosition position) override;
  bool IsCutEnabled() override;
  bool IsCopyEnabled() override;
  bool IsPasteEnabled() override;
  bool IsSelectAllEnabled() override;
  void HandleCut() override;
  void HandleCopy() override;
  void HandlePaste() override;
  void HandleSelectAll() override;

  // View
  void OnFocusChanged(bool isFocused) override;
  bool CanTakeFocus() override;

  // ScrollView
  bool UpdateOffset(const TPoint &offset) override;
  View::DescendantFocusAbility GetDescendantFocusAbility() const override;

 protected:
  enum RenderObjectIndex { kLeader = kRenderObjectCount, kRenderObjectCount };
  explicit TextInputView(std::shared_ptr<TextEditingController> editing_controller = nullptr,
                         std::shared_ptr<TextSelectionControl> selection_control = nullptr,
                         const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent());
  void OnEditingStateUpdate(const EditingState &editing_state);
  void OnKeyboardAction(KeyboardAction action);

  void Init() override;
  void Unmount() override;
  void Layout() override;
  void CalculateMatrixAndApply(TPoint offset, TScalar scale) override;
  void SetChildrenRenderObject(const std::shared_ptr<RenderObject> &children_render_object) override;
  int32_t GetTotalRenderObjectCount() override;

 private:
  void RequestKeyboard();
  void LayoutEditable();
  void LayoutPlaceholder();
  void HideToolbar();
  void AddGestureEvents();
  void DidChangeTextEditingValue();
  void HandleSelectionChanged(const TextSelection &text_selection,
                              EditableObject::SelectionChangeCause cause = EditableObject::SelectionChangeCause::kNone);
  void HandleCaretChanged(const TRect &caret_rect);
  void LoseFocus();
  void GainFocus();
  void OpenInputConnection();
  void CloseInputConnection();
  KeyboardConfiguration CreateKeyboardConfiguration();
  EditingState EditingValueToState(const TextEditingValue &value);
  void FormatAndSetValue(const TextEditingValue &value);
  void FinalizeEditing(KeyboardAction action, bool should_unfocus);
  void UpdateRemoteEditingValueIfNeeded();
  void StartOrStopCaretTimerIfNeeded();
  void StartCaretTimer();
  void StopCaretTimer();
  void OnCaretTimer();
  void UpdateOrHideSelectionOverlayIfNeeded();
  constexpr bool IsMultiLine() const { return attributes_.paragraph_style.max_lines > 1; }
  void ShowCaretOnScreen();
  bool ShouldShowToolbar();
  void ShowToolbarIfPossible();
  virtual void OnSingleTapUp(const TapDetails &details);
  virtual void OnSingleLongTapStart(const LongPressStartDetails &details);
  virtual void OnSingleLongTapMoveUpdate(const LongPressMoveUpdateDetails &details);
  virtual void OnSingleLongTapEnd(const LongPressEndDetails &details);
  virtual void OnDoubleTap(const DoubleTapDetails &details);
  virtual void OnMouseDragUpdate(const DragUpdateDetails& details);
  void MarkRenderObjectDirtyIfNeeded();
  TPoint GetOffsetToRevealCaret(TRect rect);
  void OnCaretBlinkAnimation(TScalar alpha);
  void RefreshCursorColorAlpha(TScalar alpha);
  void BringIntoViewHorizontal(TextPosition position);
  void BringIntoViewVertical(TextPosition position);

  bool show_caret_on_screen_scheduled_ = false;
  TextEditingValue old_value_;
  std::shared_ptr<Timer> caret_timer_;
  std::shared_ptr<AnimationController> animation_controller_;
  std::shared_ptr<TextSelectionOverlay> selection_overlay_;
  TextEditingValue received_remote_value_;
  TextEditingValue last_formatted_unmodified_value_;
  TextEditingValue last_formatted_value_;
  std::shared_ptr<TextInputConnection> text_input_connection_;
  std::shared_ptr<TextObject> placeholder_;
  EditableObject::Attributes editable_attributes_;
  std::shared_ptr<EditableObject> editable_;
  std::shared_ptr<ClipboardStatusNotifier> clipboard_status_;
  ValueNotifier<ClipboardStatus>::Callback clipboard_listener_;
  uint64_t clipboard_listener_handler_ = 0;
  std::shared_ptr<OffstageObject> placeholder_offstage_;
  TextStyle placeholder_style_;
  Attributes attributes_;
  EditableObject::SelectionChangedHandler selection_changed_handler_;
  bool is_caret_blink_with_animation_ = true;
  bool target_caret_visibility_ = false;
  bool force_update_to_connection_ = false;
  std::shared_ptr<LayerLink> toolbar_layer_link_;
  std::shared_ptr<LayerLink> start_handle_layer_link_;
  std::shared_ptr<LayerLink> end_handle_layer_link_;
  TRect current_caret_rect_;
  EndEditingCallback end_callback_;
  TPoint text_offset = TPoint::Make(0, 0);
  bool text_offset_needs_update = true;
  TPoint mouse_drag_start_poistion_;
  TPoint long_tap_start_position_;
  EditingState::TextAffinity text_affinity_;
  std::shared_ptr<HoverGestureRecognizer> hover_gesture_;
  std::function<void(const std::vector<std::shared_ptr<InlineSpan>>&)> text_did_change_callback_;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::TextInputView, bases<tdfcore::ScrollView>)
TDF_REFL_END(tdfcore::TextInputView)
