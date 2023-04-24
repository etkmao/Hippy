//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include "core/platform/common/channel/channel_runtime.h"
#include "core/platform/windows/event/platform_event_handler_windows.h"
#include "core/platform/windows/keyboard/keyboard_windows.h"
#include "core/platform/windows/view/tdf_view.h"
#include "core/tdfi/event_center.h"

namespace tdfcore {

class TDFWindow;

class TDFEngineWindows;

/***
 * @brief 内核输出View，提供内核Output渲染目标以及Native事件传递
 */
class TDFOutputView : public TDFView, public IDropTarget {
 public:
  /***
   * @return 构造函数
   * @param title 窗口名称
   * @param width 窗口宽带
   * @param height 窗口高度
   */
  explicit TDFOutputView(const std::weak_ptr<TDFEngineWindows> tdf_engine, const std::string_view& = "TDFOutputView",
                         const TRect& frame = TRect::MakeEmpty());
  void Init() override;
  ~TDFOutputView();
  HWND GetWindowHandle() const override;

  /**
   * @brief 设置event_center_用于发送事件到内核
   * @param event_center
   */
  void SetEventCenter(const std::shared_ptr<EventCenter>& event_center) { event_center_ = event_center; }
  /**
   * @brief 设置channel_runtime_用于获取Keyboard等对象
   * @param channel_runtime
   */
  void SetChannelRuntime(const std::shared_ptr<ChannelRuntime>& channel_runtime) { channel_runtime_ = channel_runtime; }

  void SetKeyboard(const std::shared_ptr<Keyboard>& keyboard);

  /***
   * @brief 设置平台事件处理器
   */
  void SetPlatformEventHandler(const std::shared_ptr<PlatformEventHandler>& handler) { event_handler_ = handler; }

 protected:
  LRESULT MessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) override;
  void OnImeComposition(UINT const message, WPARAM const wparam, LPARAM const lparam);
  void CreateImeWindow();
  void UpdateImeWindow(const TRect& caret_rect);
  HRESULT DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
  HRESULT DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
  HRESULT DragLeave() override;
  HRESULT Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
  HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
  ULONG AddRef() override;
  ULONG Release() override;
  void UpdateFrame(int64_t width, int64_t height);

 private:
  std::string window_title_;
  TRect frame_;
  std::shared_ptr<TDFWindow> tdf_window_;
  std::shared_ptr<EventCenter> event_center_;
  std::shared_ptr<KeyboardWindows> keyboard_;
  std::shared_ptr<ChannelRuntime> channel_runtime_;
  std::shared_ptr<PlatformEventHandler> event_handler_;
  std::weak_ptr<TDFEngineWindows> tdf_engine_;
  LONG ref_count_ = 0;
  bool need_revoke_ole_initialize_ = false;
};

}  // namespace tdfcore