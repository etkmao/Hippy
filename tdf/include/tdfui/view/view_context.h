//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/engine/graphics/device.h"
#include "core/engine/render/render_tree.h"
#include "core/support/animation/animation_scope.h"
#include "core/tdfi/shell.h"
#include "tdfui/image/image_manager.h"
#include "tdfui/view/category/mouse_manager.h"

namespace tdfcore {
class View;

class ViewContext : public Object, public std::enable_shared_from_this<ViewContext> {
 public:
  ~ViewContext() override;
  /**
   * @brief 获取当前的上下文
   * @return 指向当前上下文的指针
   */
  static std::shared_ptr<ViewContext> GetCurrent();

  /**
   * 设置默认的Build Function到Shell中，View框架的一般使用者都可使用此便捷方法
   */
  void SetupDefaultBuildFunction();

  /**
   * @brief 设置为当前的上下文
   */
  void SetCurrent();

  /**
   * @brief 获取shell
   * @return 指向shell的指针
   */
  const std::shared_ptr<Shell>& GetShell() const { return shell_; }

  /**
   * @brief 获取root view
   * @return 指向root view的指针
   */
  const std::shared_ptr<View>& GetRootView() const { return root_view_; }

  /**
   * @brief 添加需要重新Layout的视图
   * @param view 需要重新Layout的视图
   */
  void AddDirtyView(const std::weak_ptr<View>& view);

  /**
   * @brief 标记需要重新build
   */
  void MarkNeedsBuild();

  /**
   * @brief 输出一棵Render Tree
   */
  std::shared_ptr<RenderTree> BuildRenderTree();

  /*
   * @brief 获取Image Manager对象
   */
  std::shared_ptr<ImageManager> GetImageManager();

  /*
   * @brief 获取AnimationScopeManager对象
   */
  std::shared_ptr<AnimationScopeManager> GetAnimationScopeManager();

 protected:
  explicit ViewContext(std::shared_ptr<Shell> shell);
  void Init() override;

 private:
  void FlushLayout();
  void UpdateViewportMetrics(const ViewportMetrics& viewport_metrics);
  std::shared_ptr<Shell> shell_;
  std::shared_ptr<MouseManager> mouse_manager_;
  std::shared_ptr<RenderTree> render_tree_;
  std::list<std::weak_ptr<View>> dirty_views_;
  std::unordered_map<uint64_t, std::list<std::weak_ptr<View>>::iterator> dirty_view_iterator_map_;
  std::shared_ptr<View> root_view_;
  std::shared_ptr<ImageManager> image_manager_;
  std::shared_ptr<AnimationScopeManager> animation_scope_manager_;
  uint64_t task_driver_event_callback_id_ = 0;
  uint64_t metrics_listener_id_ = 0;
  std::optional<OrientationEvent::Orientation> orientation_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
