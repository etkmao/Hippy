//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <memory>
class GrDirectContext;
namespace tgfx {
class Context;
}  // namespace tgfx

namespace tdfcore {
class Device;
class DeviceSharedContext;

// skia/include/gpu/GrTypes.h
enum GrGLBackendState {
    kRenderTarget_GrGLBackendState     = 1 << 0,
    kTextureBinding_GrGLBackendState   = 1 << 1,
    kView_GrGLBackendState             = 1 << 2,
    kBlend_GrGLBackendState            = 1 << 3,
    kMSAAEnable_GrGLBackendState       = 1 << 4,
    kVertex_GrGLBackendState           = 1 << 5,
    kStencil_GrGLBackendState          = 1 << 6,
    kPixelStore_GrGLBackendState       = 1 << 7,
    kProgram_GrGLBackendState          = 1 << 8,
    kFixedFunction_GrGLBackendState    = 1 << 9,
    kMisc_GrGLBackendState             = 1 << 10,
    kALL_GrGLBackendState              = 0xffff
};

class GraphicsContext {
 public:
  std::shared_ptr<Device> owner_device;
  tgfx::Context *tgfx_context = nullptr;
  DeviceSharedContext * shared_context = nullptr;

#if TDF_INCLUDE_SKIA
  GrDirectContext *gr_context = nullptr;
#endif
  static GraphicsContext *GetCurrent();

  void ResetContext(uint32_t state = kALL_GrGLBackendState);
};

}  // namespace tdfcore
