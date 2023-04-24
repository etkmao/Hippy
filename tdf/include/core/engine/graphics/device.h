//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include <vector>
#include <future>
#include "core/common/id_generator.h"
#include "core/common/object.h"

class GrDirectContext;

namespace tdfcore {
class GraphicsContext;
class Atlas;
class Device;

enum class RenderBackend {
    kSKIA,
    kTGFX,
};

void SetRenderBackend(const RenderBackend& backend);

RenderBackend GetRenderBackend();
/**
 * 记录一个渲染相关的指令
 */
struct RenderCommand {
  int64_t id = IDGenerator::NextUniqueID();
  std::string debug_info;
  std::function<void(GraphicsContext* graphics_context)> exec;
};

/**
 * 记录一个渲染指令队形，由Device间接持有(通过DeviceSharedContext)和管理(AddRenderCommand / FlushRenderCommand)
 */
class RenderCommandQueue : public Object {
 public:
  /**
   * @brief 按序执行队列中的渲染指令，并在执行完全部指令后清空队列。要求在Raster线程
   */
  void Flush();

  /**
   * @brief 记录一个渲染指令，在必要时执行，由框架保证执行的时机和上下文的正确性
   */
  void Add(const RenderCommand& command, const std::shared_ptr<Device>& device);


 private:
  using CommandGroup = std::pair<std::shared_ptr<Device>, std::vector<RenderCommand>>;
  // 目前用 std::vector 就够了
  std::vector<CommandGroup> command_queue;
  std::shared_ptr<Device> current_device;
  std::mutex mutex_;
  FRIEND_OF_TDF_ALLOC
};

/**
 * @brief 创建Device可以共享的上下文信息
 */
class DeviceSharedContext : public Object {
 public:
  // 共享GLContext
  void* gl_context = nullptr;
  // 共享Atlas缓存数据，目前主要缓存文字
  std::shared_ptr<Atlas> atlas;
  std::shared_ptr<RenderCommandQueue> render_command_queue_;
};

class Device : public Object, public std::enable_shared_from_this<Device> {
 public:
  /**
   * @brief 创建一个硬件设备对象，用于管理图形环境
   * @param native_window 如果提供此参数则Device内部持有的EGLSurface是EGLWindowSurface，否则是EGLPbufferSurface。
   *        对于mesa的DRM EGL平台，Pbuffer暂不支持，故必需使用EGLWindowSurface:
   * https://gitlab.freedesktop.org/mesa/mesa/-/issues/262
   */
  static std::shared_ptr<Device> Make(const std::shared_ptr<DeviceSharedContext>& shared_context = nullptr,
                                      void* native_window = nullptr);

  /**
   * @brief 创建一个虚拟硬件设备对象，用于单元测试
   */
  static std::shared_ptr<Device> MakeMock();

  /**
   * @brief 在当前Device记录一个渲染指令（比如纹理创建），并在合适时机执行。如果当前有可用的图形环境，则立即执行。
   */
  virtual void ExecuteOrPostRenderCommand(const RenderCommand& command) = 0;

  /**
   * @brief 锁定该线程环境，使得后续图形API调用有效。
   *        TODO(vimerzhao): 改造完成后尝试改成private级别。
   * @return 返回图形环境
   */
  virtual GraphicsContext* Lock() = 0;

  /**
   * @brief 解锁该线程环境，GraphicsContext将失效
   */
  virtual void Unlock() = 0;

  /**
   * @brief 当前设备的最大渲染目标尺寸
   * @return 返回最大尺寸
   */
  virtual int maxRenderTargetSize() = 0;

  virtual std::shared_ptr<DeviceSharedContext> GetSharedContext() { return shared_context_; }

  /**
   * @brief 提交绘制指令
   */
  virtual void Flush(bool sync_cpu = true) = 0;

 protected:
  std::shared_ptr<DeviceSharedContext> shared_context_;
};

}  // namespace tdfcore
