//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <atomic>
#include <vector>
#include "core/engine/graphics/board.h"
#include "core/engine/graphics/device.h"
#include "core/engine/raster/layer_tree.h"
#include "core/platform/common/output/embedded_delegate.h"
#include "core/platform/common/view/platform_view.h"

namespace tdfcore {
class Scheduler;

/**
 * @brief 动态化框架的渲染结果输出抽象类，动态化框架渲染内核在每次绘制时询问Output获取
 *        画板(Board)进行绘制，绘制结束后调用`Present`完成输出。每个Board包含一个
 *        Main Surface 和 0~n个 Overlay Surface。
 */
class Output : public Object, public std::enable_shared_from_this<Output> {
 public:
  ~Output() override = default;
  /**
   * @brief 创建一个Board(Main Surface + Overlay Surface)，用于渲染。
   */
  virtual std::shared_ptr<Board> MakeBoard() = 0;

  /**
   * @brief Output是否有有效的后端用于执行MakeBoard
   */
  virtual bool IsValid() = 0;

  /**
   * @brief 尝试初始化后端，不一定成功（比如平台窗口确实没有创建好时）
   */
  virtual void InitBackend() = 0;
  /**
   * @brief see EmbeddedDelegate::UpdateEmbeddedInfo
   */
  std::future<void> UpdateEmbeddedInfo(const std::vector<EmbeddedInfo> &embedded_infos);

  /**
   * @brief 提交绘制
   */
  void Present();

  void PresentEmbeddedView();

  virtual void PresentMainView() = 0;

  virtual void PresentOverlayViews() = 0;

  std::shared_ptr<EmbeddedDelegate> GetEmbeddedDelegate() const { return embedded_delegate_; }

  virtual std::shared_ptr<Device> GetDevice() const { return nullptr; };

  virtual void UpdateDevice(const std::shared_ptr<Device>& device) = 0;

 protected:
  Output(const std::shared_ptr<Scheduler> &scheduler,
         const std::shared_ptr<EmbeddedDelegate> &embedded_delegate);
  std::shared_ptr<Board> board_;
  std::shared_ptr<Scheduler> scheduler_;
  std::shared_ptr<EmbeddedDelegate> embedded_delegate_;
};
}  // namespace tdfcore
