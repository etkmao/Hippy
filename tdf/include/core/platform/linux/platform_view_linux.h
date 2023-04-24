//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <fcntl.h>
#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "core/common/listener.h"
#include "core/common/rect.h"
#include "core/platform/common/view/platform_view.h"

namespace tdfcore {

class PlatformViewLinux : public PlatformView {
 public:
  using PlatformView::PlatformView;
  void SetFrame(const TRect &frame) override;
  TRect GetFrame() override;
  void InsertView(const std::shared_ptr<PlatformView> &platform_view, int64_t index) override;
  void RemoveView(const std::shared_ptr<PlatformView> &platform_view) override;
  void *GetViewPointer() override;
  void InitWindow(const std::shared_ptr<Device> &shared_device) override;

  virtual void Present(GraphicsContext *graphics_context) override;

 protected:
  virtual EGLNativeWindowType GetNativeWindow() = 0;

 private:
  TRect window_frame;

  FRIEND_OF_TDF_ALLOC;
};

// if TDF_SUPPORT_X11
class PlatformViewLinuxX11 : public PlatformViewLinux {
 public:
  using PlatformViewLinux::PlatformViewLinux;
  void InitNativeWindow(TRect frame);

 protected:
  EGLNativeWindowType GetNativeWindow() override { return native_window_; };

 private:
  EGLNativeWindowType native_window_;
};
// end

struct drm_fb {
  struct gbm_bo *bo;
  uint32_t fb_id;
};
class PlatformViewLinuxDRM : public PlatformViewLinux {
 public:
  using PlatformViewLinux::PlatformViewLinux;
  /**
   * DRM模式下只支持全屏
   */
  void InitNativeWindow();

  void *GetResourceWindow();

  void Present(GraphicsContext *graphics_context) override final;

 protected:
  EGLNativeWindowType GetNativeWindow() override { return native_window_; };

 private:
  void InitDRM();
  void InitGBM();
  uint32_t FindCrtcForConnector(const drmModeRes *resources, const drmModeConnector *connector);

  struct drm_fb *GetDRMFbFromBo(struct gbm_bo *bo);
  struct {
    int fd;
    drmModeModeInfo *mode;
    uint32_t crtc_id;
    uint32_t connector_id;
  } drm_info_;
  struct {
    struct gbm_device *dev;
    struct gbm_surface *surface_0;
    struct gbm_surface *surface;
  } gbm_info_;

  EGLNativeWindowType native_window_;
};
//

}  // namespace tdfcore
