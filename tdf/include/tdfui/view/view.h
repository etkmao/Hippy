//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include <string>

#include "core/common/object.h"
#include "core/engine/render/box_border.h"
#include "core/engine/render/box_shadow.h"
#include "core/engine/render/gradient.h"
#include "core/engine/render/render_object.h"
#include "core/support/gesture/gesture_handler.h"
#include "core/support/gesture/recognizer/gesture_recognizer.h"
#include "core/support/gesture/recognizer/hover_gesture_recognizer.h"
#include "tdfui/view/view_context.h"

namespace tdfcore {
class MouseListener;

constexpr const char *kSystemCursorObjectExfield = "SystemCursor_Object";

/**
 * @brief 组件对象基类
 * 封装了组件通用接口
 */
class View : public Object,
                            public std::enable_shared_from_this<View>,
                            public Reflectable,
                            public Diagnosticable {
  TDF_REFF_CLASS_META(View)
 public:
  using OnFocusChangedListener = std::function<void(bool)>;

  /**
   * @brief 焦点传递策略
   */
  enum class DescendantFocusAbility {
    // 焦点优先询问自己
    kBefore,

    // 焦点优先询问孩子
    kAfter,

    // 焦点不向孩子传递
    kBlock
  };

  /**
   * @brief 获取viewContext
   */
  std::shared_ptr<ViewContext> GetViewContext() const;

  /**
   * @brief 设置位置及大小
   * @param frame 位置大小
   */
  void SetFrame(const TRect &frame);

  /**
   * @brief 获取位置大小
   * @return 位置大小
   */
  TRect GetFrame() const;

  /**
   * @brief 设置view是否不再绘制
   * 默认为false
   * @param is_offstage 是否不再绘制
   */
  virtual void SetOffStage(bool is_offstage);

  /**
   * @brief 获取view是否不再绘制
   * @return 是否不再绘制
   */
  virtual bool IsOffStage() const;

  /**
   * @brief 设置是否重绘边界
   * @param is_repaint_boundary 是否重绘边界
   */
  virtual void SetRepaintBoundary(bool is_repaint_boundary);

  /**
   * @brief 获取是否重绘边界
   * @return 是否重绘边界
   */
  virtual bool IsRepaintBoundary() const;

  /**
   * @brief 设置事件响应行为
   * @param behavior
   */
  virtual void SetHitTestBehavior(HitTestBehavior behavior);

  /**
   * @brief 获取事件响应行为
   * @param behavior
   */
  virtual HitTestBehavior GetHitTestBehavior() const;

  /**
   * @brief 设置变换矩阵
   * @param matrix 变换矩阵
   */
  void SetTransform(const TM33 &matrix);

  /**
   * @brief 返回变换矩阵
   * @return 变换矩阵
   */
  TM33 GetTransform();

  /**
   * @brief 获取相对于目标View坐标系的transform矩阵，如果目标View为nullptr，默认为全局坐标系
   * @param view 目标View
   * @return 转换矩阵
   */
  TM33 GetTransformToView(const std::shared_ptr<View> &view = nullptr) const;

  /**
   * @brief 设置是否需要裁剪边界
   * @param clip_to_bounds 需要裁剪边界
   */
  virtual void SetClipToBounds(bool clip_to_bounds);

  /**
   * @brief 获取是否需要裁剪边界
   * @return 是否需要裁剪边界
   */
  virtual bool GetClipToBounds() const;

  /**
   * @brief 设置裁剪区域
   * @param 裁剪区域
   */
  virtual void SetClipRect(const TRect &clip_rect);

  /**
   * @brief 设置裁剪区域
   * @return 裁剪区域
   */
  virtual TRect GetClipRect() const;

  /**
   * @brief 设置裁剪路径
   * @param clip_path 裁剪路径
   */
  virtual void SetClipPath(const std::shared_ptr<Path> &clip_path);

  /**
   * @brief 获取裁剪路径
   * @return 裁剪路径
   */
  virtual std::shared_ptr<Path> GetClipPath() const;

  /**
   * @brief 设置透明度
   * @param opacity 透明度
   */
  void SetOpacity(float opacity);
  /**
   * @brief 获取透明度
   * @return 透明度
   */
  float GetOpacity() const;

  /**
   * @brief 设置背景颜色
   * @param color 背景颜色
   */
  void SetBackgroundColor(Color color);

  /**
   * @brief 获取背景颜色
   * @return 背景颜色, 默认透明色
   */
  Color GetBackgroundColor() const;

  /**
 * @brief 设置鼠标悬浮指针类型
 * @param Cursor 悬浮指针类型
 */
  void SetHoverCursor(const SystemMouseCursor &cursor);

  /**
   * @brief 获取鼠标悬浮指针类型
   * @param Cursor 悬浮指针类型
   */
  SystemMouseCursor GetHoverCursor() const;

  /**
   * @brief 设置渐变背景
   * @param gradient 渐变
   */
  virtual void SetGradient(const std::shared_ptr<Gradient> &gradient);

  /**
   * @brief 获取渐变背景
   * @return 渐变, 默认nullptr
   */
  virtual std::shared_ptr<Gradient> GetGradient() const;

  /**
   * @brief 设置阴影
   * @param shadow 阴影
   */
  virtual void SetShadow(const BoxShadow &shadow);

  /**
   * @brief 获取阴影
   */
  virtual BoxShadow GetShadow() const;

  /**
   * @brief 设置边框
   * @param border 边框
   */
  virtual void SetBorder(const BoxBorder &border);

  /**
   * @brief 获取边框
   */
  virtual BoxBorder GetBorder() const;

  /**
   * @brief 设置圆角尺寸
   * @param radii 圆角尺寸, 分别对应4个角的x、y，共计8个值
   */
  virtual void SetRadius(const std::array<float, 8> &radii);

  /**
   * @brief 获取圆角尺寸
   */
  virtual const std::array<float, 8> &GetRadius() const;


  /**
   * @brief 设置View的Z Index，一般类Web的框架会有这个特性
   */
  void SetZIndex(int32_t z_index);

  /**
   * @brief 获取View的Z Index，一般是排序时内部使用
   */
  int32_t GetZIndex() const;

  /**
   * @brief 设置无障碍描述信息
   */
  void SetAccessibilityLabel(const std::u16string &label);

  /**
   * @brief 获取无障碍描述信息
   */
  const std::u16string &GetAccessibilityLabel() const;

  /**
   * @brief 添加手势
   * @param gesture_recognizer 需要设置的手势
   */
  void AddGesture(const std::shared_ptr<GestureRecognizer> &gesture_recognizer);

  /**
   * @brief 移除手势
   * @param gesture_recognizer 需要移除的手势
   */
  void RemoveGesture(const std::shared_ptr<GestureRecognizer> &gesture_recognizer);

  /**
   * @brief 请求获取焦点
   * @return 是否成功
   */
  virtual bool RequestFocus();

  /**
   * @brief 清除焦点
   */
  virtual void ClearFocus();

  /**
   * @brief 当前view是否允许获取焦点
   * @return 是否允许 默认false
   */
  virtual bool CanTakeFocus();

  /**
   * @brief 当前view是否持有焦点，若子类持有焦点，父类也认为持有该焦点
   * @return true 持有
   */
  virtual bool HasFocus();

  /**
   * @brief 当前view是否为焦点所处的view
   * @return true 当前view正是焦点view
   */
  virtual bool IsFocused();

  /**
   * @brief 焦点改变逻辑
   * @param isFocused 焦点状态
   */
  virtual void OnFocusChanged(bool isFocused);

  /**
   * @brief 设置焦点改变的回调
   * @param listener 回调
   */
  void SetFocusChangeListener(const OnFocusChangedListener &listener);

  /**
   * @brief 获取控件焦点传递策略
   * @return 传递策略
   */
  virtual DescendantFocusAbility GetDescendantFocusAbility() const;

  /**
   * @brief 添加子节点
   * @param child 子节点
   */
  virtual void AddView(const std::shared_ptr<View> &child);

  /**
   * @brief 添加子节点到指定位置
   * @param child 子view
   * @param index 下标
   */
  virtual void AddView(const std::shared_ptr<View> &child, int64_t index);

  /**
   * @brief 移除子view
   * @param child 子view
   */
  virtual void RemoveView(const std::shared_ptr<View> &child);

  /**
   * @brief 从父视图中移除
   */
  virtual void RemoveFromParentView();

  /**
   * @brief 将子视图移动到最顶部
   * @param child 子视图
   */
  void BringChildViewToFront(const std::shared_ptr<View> &child);

  /**
   * @brief 获取父节点
   * @return 父节点
   */
  std::shared_ptr<View> GetParent();

  /**
   * @brief 获取所有的子视图
   * @return 子视图数组
   */
  std::vector<std::shared_ptr<View>> GetChildren() const;

  /**
   * @brief 获取根Render Object
   */
  virtual std::shared_ptr<RenderObject> GetRootRenderObject() const;

  /**
   * @brief 获取该view是否被挂载
   */
  virtual bool IsMounted() const { return is_mounted_; }

  /**
   * @brief 初始化方法
   */
  void Init() override;

  /**
   * @brief 第一次被加入父视图
   * 仅被调用一次
   */
  virtual void Load();

  /**
   * @brief 被挂载到渲染树
   * 可能多次被调用
   */
  virtual void Mount();

  /**
   * @brief 布局发生改变
   * 例如View frame发生变化、被加入父视图时，不会立即调用，下一次渲染周期执行
   * 可能被调用多次
   */
  virtual void Layout();

  /**
   * @brief 从渲染树上卸载
   * 可能多次被调用
   */
  virtual void Unmount();

  /**
   * @brief 标记需要布局
   */
  void MarkNeedsLayout();

  std::shared_ptr<Diagnostics> ToDiagnostics() const override;
  TPoint LocalToGlobal(const TPoint &local, const std::shared_ptr<View> &ancestor = nullptr) const;
  TPoint GlobalToLocal(const TPoint &global, const std::shared_ptr<View> &ancestor = nullptr) const;

  /**
   * @brief 设置是否需要开启调试绘制模式
   * @param enable
   * @param is_recursive 是否递归所有子节点开启调试绘制模式
   */
  void SetDebugPaintEnable(bool enable, bool is_recursive = true) const;

  void SetRasterCachePolicy(RasterCachePolicy cache_policy);

  std::shared_ptr<RenderObject> GetRenderObject(int index);

 protected:
  explicit View(const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent());
  void SetParent(const std::shared_ptr<View> &parent) { parent_ = parent; }
  std::shared_ptr<RenderObject> GetFirstRenderObject() const;
  std::shared_ptr<RenderObject> GetLastRenderObject() const;
  virtual int32_t GetTotalRenderObjectCount();
  virtual void SetChildrenRenderObject(const std::shared_ptr<RenderObject> &children_render_object);
  int32_t FindPreRenderObject(int32_t start_index);
  int32_t FindNextRenderObject(int32_t start_index);
  void AddRenderObject(int32_t index, const std::shared_ptr<RenderObject>& render_object);
  void RemoveRenderObject(int32_t index);
  void AddDirtyViewRecursively();
  bool RequestFocusForSelf();
  bool RequestFocusForChild();
  bool IsAncestorBlocksFocus();
  void UpdateWhenChildFocused(const std::shared_ptr<View> &child);
  void UpdateWhenChildUnfocused();
  void HandleFocusGain();
  void ClearFocusSelf(bool notify_ancestor);
  void UpdateBorderObjectByRadius();
  void UpdateShadowObjectByRadius();

  virtual void InternalSetOpacity(float opacity);
  virtual float InternalGetOpacity() const;
  virtual void InternalSetFrame(const TRect &frame);
  virtual TRect InternalGetFrame() const;
  virtual void InternalSetTransform(const TM33 &matrix);
  virtual TM33 InternalGetTransform();
  virtual void InternalSetRenderBackgroundColor(Color color);
  virtual Color InternalGetRenderBackgroundColor() const;

  enum RenderObjectIndex {
    kOffStage,
    kRepaintBoundary,
    kOpacity,
    kTransform,
    kClipRect,
    kClipPath,
    kBorder,
    kShadow,
    kClipRRect,
    kColor,
    kRenderObjectCount
  };
  std::vector<std::shared_ptr<RenderObject>> render_objects_;
  int32_t last_render_object_index_ = kOffStage;
  std::shared_ptr<RenderObject> multi_child_render_object_;
  std::weak_ptr<View> parent_;
  std::vector<std::shared_ptr<View>> children_;
  bool is_loaded_ = false;
  bool is_mounted_ = false;
  bool needs_layout_ = true;
  std::shared_ptr<View> focused_child_;
  OnFocusChangedListener focus_changed_listener_;
  bool is_focused_ = false;
  std::u16string accessibility_label_;
  std::array<float, 8> radius_{};
  bool clip_to_bounds_ = false;
  std::weak_ptr<ViewContext> view_context_;

 private:
  int32_t z_index_ = 0;
  std::shared_ptr<HoverGestureRecognizer> hover_gesture_;
  void LayoutSelf();

  /**
   * Layout之后基于z-index重排 Render Tree (不能在Layout之前，只能影响绘制，
   * 即Render Tree 的兄弟节点的排序，不能影响布局)
   */
  void SortRenderTreeByZIndex();
  friend class ViewContext;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::View, bases<>)
TDF_REFL_END(tdfcore::View)
