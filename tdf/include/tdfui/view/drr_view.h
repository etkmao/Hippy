//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "tdfui/view/custom_paint_view.h"
#include "tdfui/view/edge_insets.h"
#include "core/support/gesture/recognizer/pan_gesture_recognizer.h"
#include "core/support/gesture/recognizer/hover_gesture_recognizer.h"

namespace tdfcore {

/**
 * 拖拽(Drag)、调整大小(Resize)、旋转(Rotate)辅助视图：DRRView
 * 被辅助的视图称为client view，client view与DRRView之间不需要有父子关系，它们通过全局坐标系进行位置关联。
 *
 * 设计师一般会对DRRView的呈现效果做出定制，开发者可通过CustomPaintView::SetPaintFunction来进行自绘。
 * 另外，对于自绘的DRRView，开发者可能还需要重载GetClientEdgeInsets方法。
 */
class DRRView : public CustomPaintView {
 public:
  /**
   * 操作把手类型，不同类型的把手具有不同的处理逻辑
   */
  enum class HandleType {
    kNone, kMove, kLeft, kTop, kRight, kBottom, kLeftTop, kRightTop, kLeftBottom, kRightBottom, kRotate
  };
  using DragStartDelegate = std::function<void(const DragStartDetails &details)>;
  using DragUpdateDelegate = std::function<void(const DragUpdateDetails &details)>;

  static std::shared_ptr<DRRView> MakeDefault();

  void Init() override;
  void Mount() override;
  HandleType GetDragHandleType() const { return drag_handle_type_; }

  /**
   * 拖拽开始、位置更新时的处理回调，使用者可以使用自定义处理逻辑代替默认处理逻辑
   * @param delegate
   */
  void SetDragStartDelegate(const DragStartDelegate &delegate) { drag_start_delegate_ = delegate; }
  void SetDragUpdateDelegate(const DragUpdateDelegate &delegate) { drag_update_delegate_ = delegate; }

  /**
   * 设置调整大小时的最大、最小尺寸
   */
  void SetMinSize(const TSize &sz) { min_size_ = sz; }
  void SetMaxSize(const TSize &sz) { max_size_ = sz; }

  /**
   * 返回DRRView与client view的边距。
   * DRRView一般会具有边框、以及操作把手（例如用于位于四个角落用于调整大小的圆点），这些都会占据一定的空间。
   * 设计师一般会对边框、操作把手的大小做出定制，因此需要提供这个接口，以便开发者能够定制。
   */
  virtual EdgeInsets GetClientEdgeInsets() const;

  /**
   * 获取当前状态下client view的全局Frame
   */
  TRect GetClientGlobalFrame() const;


  /**
   * 设置client view，DRRView将辅助client view进行拖拽(Drag)、调整大小(Resize)、旋转(Rotate)
   * client view与DRRView之间不需要有父子关系，它们通过全局坐标系进行位置关联
   */
  virtual void SetClientView(const std::shared_ptr<View> &client);

  /**
   * 设置拖拽边界（在全局坐标系下），client view在拖拽、调整大小时将不会超出边界。
   */
  void SetGlobalDragBounds(const TRect &global_bounds) { global_drag_bounds_ = global_bounds; }

  /**
   * 根据当前DRRView的状态更新client view的位置、大小
   */
  virtual void UpdateClientViewFrameWithDRRFrame();

  /**
   * 根据当前client view的位置更新DRRView
   */
  void UpdateFrameWithClientViewFrame();

  /**
   * 设置是否允许移动、改变大小
   * @param enabled
   */
  void SetMoveEnabled(bool enabled) { move_enabled_ = enabled; }
  void SetResizeEnabled(bool enabled) { resize_enabled_ = enabled; }

  virtual void HandleDragStartEvent(const DragStartDetails &details);
  virtual void HandleDragUpdateEvent(const DragUpdateDetails &details);
  virtual void HandleDragEndEvent(const DragEndDetails &details);
  virtual void HandleDragCancelEvent(const DragCancelDetails &details);

 protected:
  void RegisterGesture();
  virtual HandleType DetermineDragHandleType(const TPoint &local_pt);

  std::shared_ptr<PanGestureRecognizer> drag_recognizer_;
  std::shared_ptr<HoverGestureRecognizer> hover_recognizer_;
  HandleType drag_handle_type_ = HandleType::kNone;
  TPoint drag_start_point_;
  TRect drag_start_frame_;
  TRect global_drag_bounds_ = TRect::MakeLTRB(std::numeric_limits<TScalar>::min(),
                                              std::numeric_limits<TScalar>::min(),
                                              std::numeric_limits<TScalar>::max(),
                                              std::numeric_limits<TScalar>::max());
  DragStartDelegate drag_start_delegate_;
  DragUpdateDelegate drag_update_delegate_;
  TSize min_size_ = TSize::Make(0, 0);
  TSize max_size_ = TSize::Make(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
  std::weak_ptr<View> client_view_;
  std::shared_ptr<CustomPainter> painter_;
  bool move_enabled_ = true;
  bool resize_enabled_ = true;

 private:
  void AdjustFrameLeft(TRect &frame, const TPoint &total_drag_delta);
  void AdjustFrameTop(TRect &frame, const TPoint &total_drag_delta);
  void AdjustFrameRight(TRect &frame, const TPoint &total_drag_delta);
  void AdjustFrameBottom(TRect &frame, const TPoint &total_drag_delta);
  void AdjustFrameOffset(TRect &frame, const TPoint &total_drag_delta);
  void UpdateFrame(const DragUpdateDetails &details);
  void UpdateHoverCursorStyle(HandleType type);
  void UpdateCursorStyleOnDragReleased(const TPoint &release_global_pt);
};

}  // namespace tdfcore
