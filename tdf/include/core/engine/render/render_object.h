//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
#include "core/common/tree_node.h"
#include "core/engine/hittest/hit_test.h"
#include "core/engine/raster/painting_context.h"
#include "core/engine/raster/raster_cache.h"
#include "core/engine/render/render_context.h"
#include "core/engine/render/render_property.h"
#include "core/engine/render/debug_info.h"

namespace tdfcore {

class Layer;
class RenderContext;

/**
 * @brief 渲染对象基类
 * 封装了基本的渲染对象通用接口
 */
class RenderObject : public TreeNode<RenderObject, RenderContext> {
  TDF_REFF_CLASS_META(RenderObject)

 public:
  ~RenderObject() { RemoveAll(); }
  static constexpr TPoint kZeroPoint = TPoint::Make(0, 0);

  /**
   * @brief 获取大小信息
   */
  const TSize &GetSize() const { return size_; }

  /**
   * @brief 设置大小信息
   * @param size 大小
   */
  void SetSize(const TSize &size);

  /**
   * @brief 获取偏移信息
   */
  const TPoint &GetOffset() const { return offset_; }

  /**
   * @brief 获取偏移信息
   */
  void SetOffset(const TPoint &offset);

  /**
   * @brief 遍历整个RenderTree子树
   */
  virtual void Traverse(const std::function<void(std::shared_ptr<RenderObject> &)> &fn);

  /**
   * @brief 设置相对布局信息（相对于父节点）
   * @param rect
   */
  virtual void SetLayoutInfo(const TPoint &offset, const TSize &size);

  /**
   * @brief 绘制接口
   * @param context
   */
  virtual void Paint(PaintingContext &context, TPoint offset);

  /**
   * @brief 设置是否开启debug渲染
   * 开启之后所有的render对象会有边框修饰
   */
  virtual void SetDebugPaintEnable(bool is_enabled, bool is_recursive = false);

  /**
   * @brief 静态创建debug paint对象
   * @param color
   * @return
   */
  static TPaint MakeDebugPaint();

  /**
   * @brief 获取是否是重绘边界
   */
  constexpr bool IsRepaintBoundary() const { return is_repaint_boundary_; }

  /**
   * @brief 获取是否需要绘制
   */
  constexpr bool NeedsPaint() const { return needs_paint_; }

  /**
   * @brief 获取是否合成属性
   */
  constexpr bool NeedsCompositing() const { return needs_compositing_; }

  /**
   * @breif 设置是否重绘边界
   */
  void SetRepaintBoundary(bool is_repaint_boundary) { is_repaint_boundary_ = is_repaint_boundary; }

  /**
   * @brief 节点标脏
   */
  void MarkNeedsPaint();

  /**
   * @brief 标记是否合成属性需要更新
   */
  void MarkNeedsCompositingUpdate();

  /**
   * @brief 更新合成标志，并调用标脏逻辑
   */
  virtual void UpdateCompositingFlag();

  /**
   * @brief 获取父亲节点
   */
  std::shared_ptr<RenderObject> GetParent() const;

  /**
   * @brief 获取逻辑Layer实例
   */
  virtual std::shared_ptr<Layer> GetLayer() const;

  /**
   * @brief 设置节点的图层
   * @param layer 图层
   */
  virtual void SetLayer(const std::shared_ptr<Layer> &layer);

  virtual void ApplyPaintTransform(std::shared_ptr<RenderObject> child, TM33 &transform) const;

  /**
   * @brief 获取相对祖先节点的坐标系
   * @param ancestor 祖先节点
   */
  TM33 GetTransformToAncestor(std::shared_ptr<RenderObject> ancestor);

  /**
   * @brief 局部坐标转换为全局坐标
   * @param point 当前坐标值
   * @param ancestor 祖先节点，如果祖先节点为空，则一直回溯至根节点
   */
  TPoint LocalToGlobal(const TPoint &point, std::shared_ptr<RenderObject> ancestor = nullptr);

  /**
   * @brief 全局坐标转换为局部坐标
   * @param point 当前坐标值
   * @param ancestor 祖先节点
   */
  TPoint GlobalToLocal(const TPoint &point, std::shared_ptr<RenderObject> ancestor = nullptr);

  /**
   * @brief 碰撞测试，把当前position位置中下所有 HitTestTarget 添加到 HitTestResult 中去。
   *
   * @param position 碰撞的位置，父节点坐标系
   * @return 碰撞的结果集合
   */
  virtual std::shared_ptr<HitTestResult> HitTest(const TPoint &position);

  /**
   * @brief  local_position 是否在此节点，local_position 坐标系为当前节点坐标
   *
   * @param local_position 位置
   * @return true 表示在当前节点，false 表示不在此节点
   */
  bool IsPointerInside(const TPoint &local_position) const;

  TRect GetGlobalVisibleRect();

  /**
   * 获取 HitTest 的行为方式
   * @return HitTest 的行为方式
   */
  constexpr HitTestBehavior GetHitTestBehavior() const { return hitTest_behavior_; }

  /**
   * 设置 HitTest 的行为方式
   * @param hitTest_behavior HitTest 的行为方式
   */
  void SetHitTestBehavior(HitTestBehavior hitTest_behavior) { hitTest_behavior_ = hitTest_behavior; }

  /**
   * @brief 设置点击事件出现时，该render object的区域是否被视为无穷大 true的情况 即使点击到render
   * object的区域外也可以继续传递点击事件
   * @param infinity_hit_test 是否为无穷大
   */
  void SetInfinityHitTest(bool infinity_hit_test) { infinity_hit_test_ = infinity_hit_test; }

  /**
   * @brief 获取点击区域是否被视为无穷大
   * @return 是否为无穷大
   */
  constexpr bool GetInfinityHitTest() const { return infinity_hit_test_; }

  virtual TRect GetChildVisibleRect(const RenderObject &child, const TRect &rect);

  /**
   * @brief 添加一个孩子节点，插入到容器尾部
   * @param renderObject
   */
  void Add(const std::shared_ptr<RenderObject> &renderObject);

  /**
   * @brief 插入一个孩子接到到某个孩子几点后面
   * 如果 'after' 为nullptr，则插入到容器最前面
   * 如果 'after' 不为nullptr且能找到，则插入到其后面，不能找到则直接返回
   * @param render_object 插入的孩子节点的位置
   * @param after
   */
  void Insert(const std::shared_ptr<RenderObject> &render_object, const std::shared_ptr<RenderObject> &after = nullptr);

  /**
   * @brief 移动孩子节点到某个孩子节点后面
   * @param render_object 需要移动的孩子节点
   * @param after 移动孩子节点的位置
   */
  void Move(const std::shared_ptr<RenderObject> &render_object, const std::shared_ptr<RenderObject> &after);

  /**
   * @brief 移除孩子节点
   * @param render_object
   */
  void Remove(const std::shared_ptr<RenderObject> &render_object);

  /**
   * @brief 替换孩子节点
   * @param old_object 被替换的孩子节点
   * @param new_object 替换的孩子节点
   */
  void Replace(const std::shared_ptr<RenderObject> &old_object,
               const std::shared_ptr<RenderObject> &new_object);

  /**
   * @brief 移除所有孩子节点
   */
  void RemoveAll();

  /**
   * @brief 返回clip bounds
   */
  constexpr bool IsClipToBounds() const { return is_clip_to_bounds_; }

  /**
   * @brief 设置是否clip bounds
   */
  void SetClipToBounds(bool clip);

  /**
   * @brief 判断是否所有的孩子节点都在bounds范围内
   */
  bool IsAllChildrenInsideBounds() const;

  /**
   * @brief 获得第一个child
   * @return RenderObject
   */
  std::shared_ptr<RenderObject> GetFirstChild() const;

  /**
   * @brief 获得最后一个child
   * @return RenderObject
   */
  std::shared_ptr<RenderObject> GetLastChild() const;

  /**
   * @brief 获取孩子集合
   * @return 孩子集合
   */
  std::vector<std::shared_ptr<RenderObject>> GetChildren() const;

  /**
   * @brief 获得children总个数
   * @return int
   */
  int32_t GetChildrenCount() const;

  void Attach(std::shared_ptr<RenderContext> context = nullptr) override;
  void Detach() override;
  void AdoptChild(std::shared_ptr<TreeNode> node) override;
  void DropChild(std::shared_ptr<TreeNode> node) override;
  void VisitChildren(const std::function<void(std::shared_ptr<RenderObject> &child)> &visitor) override;

  /**
   * @brief 设置纹理缓存策略（仅is_repaint_boundary_为true时生效）
   */
  void SetRasterCachePolicy(RasterCachePolicy policy);

  /**
   * @brief 获取纹理缓存策略
   */
  RasterCachePolicy GetRasterCachePolicy() const { return raster_cache_policy_; }

  /**
   * @brief 对象的调试分类信息
   */
  const char *GetDebugCategory() const override { return kRenderObjectDebugCategory; }

 protected:
  TSize size_ = TSize::MakeEmpty();
  TPoint offset_ = TPoint::Make(0, 0);
  bool is_repaint_boundary_ = false;
  bool needs_compositing_ = false;
  bool needs_compositing_update_ = false;
  RasterCachePolicy raster_cache_policy_ = RasterCachePolicy::kInherited;
  bool needs_update_raster_cache_policy_ = false;
  bool is_debug_paint_enabled_ = false;
  bool is_debug_paint_recursive_ = false;
  bool needs_paint_ = true;
  std::shared_ptr<Layer> layer_ = nullptr;
  bool infinity_hit_test_ = false;
  bool is_clip_to_bounds_ = false;
  std::shared_ptr<Layer> clip_layer_;
  std::shared_ptr<RenderObject> first_child_;
  std::shared_ptr<RenderObject> last_child_;
  std::weak_ptr<RenderObject> prev_sibling_;
  std::shared_ptr<RenderObject> next_sibling_;
  int32_t children_count_ = 0;

  // 决定 HitTest 的行为方式，默认方式为不透明
  HitTestBehavior hitTest_behavior_ = HitTestBehavior::kOpaque;
  explicit RenderObject(bool is_repaint_boundary = false);

  /**
   * @brief 绘制方法
   * @param context
   * @param offset 当前节点的绘制偏移，与[size_]成员共同组成当前绘制区域
   */
  virtual void OnPaint(PaintingContext &context, const TPoint &offset);
  virtual void OnDebugPaint(PaintingContext &context, const TPoint &offset);

  // 绘制相关的标志
  virtual bool AlwaysNeedsCompositing() { return false; }
  void SkippedPaintingOnLayer();
  void DrawBoundsSubscript(PaintingContext &context, const TRect &bound);
  std::shared_ptr<HitTestResult> HitTestSelf(const TPoint &position);
  std::shared_ptr<HitTestResult> HitTestChildren(const TPoint &position);
  void Init() override;
  std::shared_ptr<RenderObject> GetSiblingPrevious() const { return prev_sibling_.lock(); }
  std::shared_ptr<RenderObject> GetSiblingNext() const { return next_sibling_; }
  void PaintContents(PaintingContext &context, TPoint offset);
  friend class RenderContext;
  FRIEND_OF_TDF_ALLOC;
};

constexpr TRect operator&(const TPoint &offset, const TSize &size) {
  return TRect::MakeXYWH(offset.x, offset.y, size.width, size.height);
}

constexpr TRect operator&(const TSize &size, const TPoint &offset) {
  return TRect::MakeXYWH(offset.x, offset.y, size.width, size.height);
}

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::RenderObject, bases<>)
TDF_REFL_MEMBER(id_);
TDF_REFL_MEMBER(size_);
TDF_REFL_MEMBER(offset_);
TDF_REFL_MEMBER(is_clip_to_bounds_);
TDF_REFL_END(tdfcore::RenderObject)
