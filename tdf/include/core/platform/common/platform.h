//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include "core/common/macro.h"
#include "core/engine/graphics/device.h"
#include "core/engine/schedule/task_driver.h"
#include "core/platform/common/channel/channel_runtime.h"
#include "core/platform/common/clipboard.h"
#include "core/platform/common/file_manager.h"
#include "core/platform/common/file_picker.h"
#include "core/platform/common/keyboard/keyboard.h"
#include "core/platform/common/menu/menu.h"
#include "core/platform/common/platform_window.h"
#include "core/platform/common/system_mouse_cursor.h"
#include "core/platform/common/view/platform_embedded_view.h"
#include "core/platform/common/view/platform_view.h"
#include "core/tdfi/event_center.h"

namespace tdfcore {

/**
 * @brief 抽象平台相关能力，提供平台接口的跨平台类
 */
class Platform : public Object {
 public:
  virtual ~Platform();

  static std::string_view GetPlatformName();

  /**
   * @brief 返回向宿主View添加子View（通常是因EmbeddedView引起）时的起始索引，各平台略有不同
   */
  static int GetSubViewBeginIndex();

  /**
   * @brief 当前是否在平台主线程
   */
  virtual bool IsMainThread() const = 0;

  /**
   * @brief 设置当前线程的名字
   */
  static void SetThreadName(const std::string &name);

  /**
   * @brief 获取当前线程Id
   */
  static uint64_t GetCurrentThreadId();

  /**
   * @brief 创建一个驱动RenderPipeline执行的TaskDriver
   */
  virtual std::shared_ptr<TaskDriver> CreateVSyncTaskDriver() = 0;

  /**
   * @brief 创建一个驱动平台任务执行的TaskDriver
   */
  virtual std::shared_ptr<TaskDriver> CreateRunLoopTaskDriver();

  /**
   * @brief 获取平台键盘类实例
   * 该实例提供键盘相关接口能力
   */
  virtual std::shared_ptr<Keyboard> GetKeyboard();

  /**
   * @brief 设置平台键盘实例，用于设置自定义的键盘实现
   * @param keyboard 键盘实例
   */
  virtual void SetKeyboard(const std::shared_ptr<Keyboard> &keyboard);

  /***
   * @brief 创建一个Window，主要用于PC平台
   * @param title window标题
   * @param frame window大小
   * @return
   */
  virtual std::shared_ptr<PlatformWindow> MakeWindow(const std::string_view &title, const TRect &frame) {
    return nullptr;
  }

  /**
   * @brief 获取剪切板类实例
   * 该实例提供系统剪贴板的读写访问能力
   */
  virtual std::shared_ptr<Clipboard> GetClipboard();

  /**
   * @brief 获取文件管理
   * @return 文件管理
   */
  virtual std::shared_ptr<FileManager> GetFileManager() = 0;

  /**
   * @brief 获取顶部菜单栏，仅macOS和Windows平台有效
   * @return 菜单栏实例
   */
  virtual std::shared_ptr<Menu> GetMenu() { return nullptr; }

  /**
   * @brief 获取App根目录
   */
  virtual std::string GetAppRootPath() const = 0;

  /**
   * @brief 获取App cache的目录
   */
  virtual std::string GetCacheDirectoryPath() const = 0;

  /**
   * @brief 获取设备像素密度
   */
  virtual float GetDeviceRatio() const = 0;

  /**
   * @brief 是否为平板设备
   */
  virtual bool IsPadDevice() const { return false; }

  /**
   * @brief 创建EmbeddedView实例
   * @param view_type 视图类型，和注册EmbeddedView时对应
   * @param propsMap 初始化属性数据
   * @return 返回EmbeddedView实例
   */
  virtual std::shared_ptr<PlatformEmbeddedView> CreateEmbeddedView(const std::string &view_type, int64_t view_id,
                                                                   const PlatformEmbeddedView::PropsMap &propsMap) = 0;

  /**
   * @brief 创建OverlayView
   * @param view_id 与之对应的embedded_view的 view_id
   * @return 返回OverlayView实例
   */
  virtual std::shared_ptr<PlatformView> CreateOverlayView(int64_t view_id,
                                                          const std::shared_ptr<Scheduler> &scheduler) = 0;

  /**
   * @brief 设置事件中心
   * @param event_center 事件中心
   */
  void SetEventCenter(const std::shared_ptr<EventCenter> &event_center);

  /**
   * @brief 获取channel runtime
   * 现在支持 Android、iOS 能力，后续支持 macos、windows 后修改为纯虚函数
   */
  virtual std::shared_ptr<ChannelRuntime> GetChannelRuntime();

  /***
   * @brief 获取平台侧文件选择器
   */
  virtual std::shared_ptr<FilePicker> GetFilePicker();

  /**
   * @brief 设置平台侧鼠标光标
   */
  virtual void SetMouseCursorStyle(SystemMouseCursor system_mouse_cursor) = 0;

 protected:
  virtual std::shared_ptr<Keyboard> CreateKeyboard() = 0;
  virtual std::shared_ptr<Clipboard> CreateClipboard() = 0;
  virtual std::shared_ptr<FilePicker> CreateFilePicker() = 0;

  std::shared_ptr<Keyboard> keyboard_;
  std::shared_ptr<Clipboard> clipboard_;
  std::shared_ptr<FilePicker> file_picker_;
  std::shared_ptr<EventCenter> event_center_;

  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
