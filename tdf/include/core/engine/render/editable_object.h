//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/value_change_notifier.h"
#include "core/engine/raster/layers/layer_link.h"
#include "core/engine/raster/painting_context.h"
#include "core/support/text/text_base.h"
#include "core/engine/render/text_object.h"

namespace tdfcore {

/**
 * @brief 可编辑文本RenderObject
 * 提供了可编辑文本的编辑和渲染能力，实现了基本的手势交互以及键盘交互接口
 */
class EditableObject : public TextObject {
  TDF_REFF_CLASS_META(EditableObject)

 public:
  enum class SelectionChangeCause { kNone, kTap, kDoubleTap, kLongPress, kForcePress, kKeyboard, kDrag };
  using SelectionChangedHandler = std::function<void(const TextSelection &, SelectionChangeCause)>;
  using CaretChangedHandler = std::function<void(const TRect &)>;

  /**
   * @brief Editable属性数据结构
   */
  static constexpr TScalar kDefaultCursorWidth = 2.0;
  static constexpr TScalar kCaretMargin = kDefaultCursorWidth * 2;
  static constexpr Color kDefaultSelectionColor = Color({0x220000aa});
  struct Attributes {
    TScalar cursor_width = kDefaultCursorWidth;
    TScalar cursor_radius = 1.0;
    Color cursor_color = Color::Blue();
    Color background_cursor_color;
    bool show_cursor = false;
    bool float_cursor_on = false;
    bool paint_cursor_above_text = false;
    bool obscure_text = false;
    TextSelection selection;
    Color selection_color = kDefaultSelectionColor;
    std::shared_ptr<TextRange> prompt_rect_range = nullptr;
    Color prompt_rect_color;
    TScalar device_pixel_ratio = 1.0;
    std::shared_ptr<LayerLink> start_handle_layer_link = nullptr;
    std::shared_ptr<LayerLink> end_handle_layer_link = nullptr;
  };

  /**
   * @brief 设置属性
   * @param attributes 属性
   */
  void SetAttributes(const Attributes &attributes);
  const constexpr Attributes &GetAttributes() const { return attributes_; }

#define DeclareEditableAttribute(type, name, class_member)                           \
  constexpr const type &Get##name##Attr() const { return attributes_.class_member; } \
  void Set##name##Attr(const type &value) {                                          \
    if (attributes_.class_member == value) {                                         \
      return;                                                                        \
    }                                                                                \
    attributes_.class_member = value;                                                \
    MarkNeedsPaint();                                                                \
  }

  /// 宏定义实现Set/Get方法
  DeclareEditableAttribute(TScalar, CursorWidth, cursor_width);
  DeclareEditableAttribute(TScalar, CursorRadius, cursor_radius);
  DeclareEditableAttribute(Color, CursorColor, cursor_color);
  DeclareEditableAttribute(Color, BackgroundCursorColor, background_cursor_color);
  DeclareEditableAttribute(bool, ShowCursor, show_cursor);
  DeclareEditableAttribute(bool, FloatCursorOn, float_cursor_on);
  DeclareEditableAttribute(bool, PaintCursorAboveText, paint_cursor_above_text);
  DeclareEditableAttribute(TextSelection, Selection, selection);
  DeclareEditableAttribute(Color, SelectionColor, selection_color);
  DeclareEditableAttribute(std::shared_ptr<TextRange>, PromptRectRange, prompt_rect_range);
  DeclareEditableAttribute(Color, PromptRectColor, prompt_rect_color);
  DeclareEditableAttribute(TScalar, DevicePixelRatio, device_pixel_ratio);

#undef DeclareEditableAttribute

  /**
   * @brief 根据位置选中单词的边缘
   * @param at 位置
   * @param cause 事件类型
   */
  void SelectPositionAt(const TPoint &at, SelectionChangeCause cause);

  /**
   * @brief 选中起点到结束的区域
   * @param from 起始位置
   * @param to 结束位置
   * @param cause 事件类型
   */
  void SelectRange(const TPoint &from, const TPoint &to, SelectionChangeCause cause);

  /**
   * @brief 根据at文字选中单词
   * @param at 文字
   * @param cause 事件类型
   */
  void SelectWordAtPosition(const TPoint &pos, SelectionChangeCause cause);
  /**
   * 选中从起始到结束位置的所有单词区域
   * @param from 起始位置
   * @param to 结束位置
   * @param cause 事件类型
   */
  void SelectWordsInRange(const TPoint &from, const TPoint &to, SelectionChangeCause cause);
  /**
   * @brief Selection起始位置是否在文本区域内
   * @return bool可变类型
   */
  constexpr ValueNotifier<bool> &GetSelectionEndInViewport() { return selection_end_in_viewport_; }

  /**
   * @brief Selection结束位置是否在文本区域内
   * @return bool可变类型
   */
  constexpr ValueNotifier<bool> &GetSelectionStartInViewport() { return selection_start_in_viewport_; }

  /**
   * @brief 得到文本高度
   * @return 高度
   */
  TScalar GetPreferredLineHeight();

  /**
   * @brief 返回坐标对应的字形位置
   * @param point 坐标
   * @return 字形坐标
   */
  TextPosition GetPositionForPoint(const TPoint &point);
  /**
   * @brief 给定selection，返回端点处的本地坐标
   * @param selection 选中区域
   * @return 坐标列表
   */
  std::vector<TextSelectionPoint> GetEndPointsForSelection(const TextSelection &selection);

  /**
   * @brief 给点字形位置，返回本地Rect坐标信息
   * @param caret_position 字形位置
   * @return rect
   */
  TRect GetLocalRectCaret(const TextPosition &caret_position);

  /**
   * @brief 设置布局信息
   * 每次布局信息更新之后需要更新光标信息
   */
  void SetLayoutInfo(const TPoint &offset, const TSize &size) override;

  void SetCaretChangedHandler(CaretChangedHandler handler);
  void SetSelectionChangeHandler(SelectionChangedHandler handler);

  void SetViewportOffset(TPoint offset);

  TScalar GetMaxScrollExtend();

  TextAxis GetAxis() const {
    return rich_text_->GetLineCount() != 1 ? TextAxis::kVertical : TextAxis::kHorizontal;
  }

  /****
   * @brief 根据光标offset位置和原型rect获取光标Rect信息
   * @param caret_position
   * @param caret_prototype
   * @return
   */
  TPoint GetOffsetForCaret(const TextPosition &caret_position, const TRect &caret_prototype);
  /****
   * @brief 根据光标offset位置和原型rect获取光标高度信息
   * @param caret_position
   * @param caret_prototype
   * @return
   */
  TScalar GetFullHeightForCaret(const TextPosition &caret_position, const TRect &caret_prototype);

  /**
   * @brief 获取范围内的所有bounds
   * @param start
   * @param end
   * @return
   */
  std::vector<TRect> GetRectsForRange(uint32_t start, uint32_t end) { return rich_text_->GetRectsForRange(start, end); }

  void Layout(uint64_t width, uint64_t height = INT64_MAX) override;

 protected:
  /**
   * @brief 构造函数
   * @param text_painter
   * @param attributes
   */
  explicit EditableObject(const Attributes &attributes);

 private:
  Attributes attributes_;
  bool obscure_text_ = false;
  TRect last_caret_rect_ = TRect::MakeEmpty();
  ValueNotifier<bool> selection_start_in_viewport_ = ValueNotifier<bool>(true);
  ValueNotifier<bool> selection_end_in_viewport_ = ValueNotifier<bool>(true);
  CaretChangedHandler caret_changed_handler_ = nullptr;
  SelectionChangedHandler selection_changed_handler_ = nullptr;
  TPoint viewport_offset_ = TPoint::Make(0, 0);
  std::shared_ptr<Layer> clip_rect_layer_;
  TextPosition previous_caret_position_;
  bool need_recompute_caret_ = true;
  TRect previous_caret_prototype_;
  TRect compute_caret_rect_;
  std::shared_ptr<RichText> prototype_text_ = nullptr;
  /// 计算相关
  TPoint GetPixelPerfectCursorOffset(const TRect &caret_rect) const;
  void UpdateSelectionExtentsVisibility(const TPoint &effective_offset);
  bool ShouldShowSelectionHandle(const TextSelectionPoint &point);

  /// 绘制相关
  void PaintSelection(PaintingContext &context, const TPoint &paint_offset);
  void PaintCaret(PaintingContext &context, const TPoint &paint_offset);
  void PaintFloatingCaret(PaintingContext &context, const TPoint &paint_offset);
  void PaintSelectionHandleLayers(PaintingContext &context, const std::vector<TextSelectionPoint> &end_points);
  void OnPaint(PaintingContext &context, const TPoint &offset) override;

  /// 手势相关
  void HandleSelectionChange(const TextSelection &new_selection, SelectionChangeCause cause);
  TextSelection SelectWordAtOffset(const TextPosition &position);

  /// 键盘相关
  TextRange GetNextWord(int64_t offset) const;
  TextRange GetPreviousWord(int64_t offset) const;
  bool OnlyWhiteSpace(const TextRange &range) const;
  static bool IsWhiteSpace(int64_t code);

  TPoint GetPaintOffset() const {
    return GetAxis() == TextAxis::kHorizontal ? TPoint::Make(-viewport_offset_.x, 0)
                                              : TPoint::Make(0, -viewport_offset_.y);
  }
  void PaintContents(PaintingContext &context, TPoint offset);

  TRect GetRectFromDownstream(int64_t offset, TRect caret_prototype);
  TRect GetRectFromUpstream(int64_t offset, TRect caret_prototype);
  TRect ComputeCaretMetrics(const TextPosition &text_position, const TRect &caret_prototype);
  void BuildPrototypeText();
  TRect GetCaretPrototype(TScalar prototype_width);
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::EditableObject, bases<tdfcore::TextObject>)
TDF_REFL_MEMBER(obscure_text_);
TDF_REFL_MEMBER(last_caret_rect_);
TDF_REFL_MEMBER(viewport_offset_);
TDF_REFL_END(tdfcore::EditableObject)
