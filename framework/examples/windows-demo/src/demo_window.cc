//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#include "demo_window.h"

#include <filesystem>
#include <optional>

#include "core/platform/common/tdf_engine.h"

using namespace tdfcore;

DemoWindow::DemoWindow()
    : tdf_engine_(TDF_MAKE_SHARED(TDFEngineWindows,
                                  TDFEngineConfig{.thread_mode_ = TDFEngineConfig::ThreadModel::Performance})) {}

DemoWindow::~DemoWindow() {}

void DemoWindow::Initialize(const std::string_view& title, const TRect& frame, DWORD style) {
  TDFWindow::Initialize(title, frame, style);
  tdf_engine_->GetOutputView()->SetFrame(TRect::MakeWH(frame.Width(), frame.Height()));
  tdf_engine_->Start();
  AddView(tdf_engine_->GetOutputView().get());
  ShowWindow(tdf_engine_->GetOutputView()->GetWindowHandle(), SW_SHOW);
}

void DemoWindow::SetTDFRenderManager(std::shared_ptr<hippy::render::tdf::TDFRenderManager> tdf_render_manager) {
  tdf_render_manager_ = tdf_render_manager;
}

void DemoWindow::SetDismissHandler(const std::function<void()>& handler) { dismiss_handler_ = handler; }

LRESULT DemoWindow::MessageHandle(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept {
  switch (message) {
    case WM_CREATE: {
      auto shell = tdf_engine_->GetShell();
      tdf_render_manager_->RegisterShell(root_id_, shell);
      return 0;
    }
    case WM_SIZE: {
      int64_t width = LOWORD(lparam);
      int64_t height = HIWORD(lparam);
      tdf_engine_->GetOutputView()->SetFrame(TRect::MakeWH(width, height));
      return 0;
    }
    case WM_DESTROY: {
      auto result = DefWindowProc(GetWindowHandle(), message, wparam, lparam);
      if (dismiss_handler_) {
        dismiss_handler_();
      }
      return result;
    }
    default:
      return DefWindowProc(GetWindowHandle(), message, wparam, lparam);
  }
}

void DemoWindow::SetupDemoApp() {
  //   auto shell = engine_->GetShell();
  // 添加视图变化事件的监听，在size发生变化的时候更新navigator大小
  //   shell->GetEventCenter()->AddListener(
  //       ViewportEvent::ClassType(), [WEAK_THIS](const std::shared_ptr<Event>& event, uint64_t id) {
  //         auto self = std::static_pointer_cast<DemoWindow>(weak_this.lock());
  //         if (self) {
  //           self->engine_->GetShell()->GetUITaskRunner()->PostTask([event, weak_this] {
  //             DEFINE_AND_CHECK_SELF(DemoWindow)
  //             auto viewport_event = std::static_pointer_cast<ViewportEvent>(event);
  //             auto viewport_metrics = viewport_event->GetViewportMetrics();
  //             self->navigator_->SetFrame(tdfcore::TRect::MakeWH(
  //                 viewport_metrics.width / viewport_metrics.device_pixel_ratio,
  //                 viewport_metrics.height / viewport_metrics.device_pixel_ratio));
  //           });
  //         }
  //         return EventDispatchBehavior::kContinue;
  //       });
  //   view_context_ = TDF_MAKE_SHARED(ViewContext, shell);
  //   view_context_->SetCurrent();
  //   navigator_ = TDF_MAKE_SHARED(Navigator, HomePage::Make());
  //   auto root_view_frame = view_context_->GetRootView()->GetFrame();
  //   navigator_->SetFrame(TRect::MakeWH(root_view_frame.Width(), root_view_frame.Height()));
  //   view_context_->GetRootView()->AddView(navigator_);
  //   auto url_image_loader = TDF_MAKE_SHARED(HttpImageLoader);
  //   view_context_->GetImageManager()->GetImageLoadManager()->RegisterImageLoader("http", url_image_loader);
  //   view_context_->GetImageManager()->GetImageLoadManager()->RegisterImageLoader("https", url_image_loader);
  //   shell->SetBuildFunction([WEAK_THIS]() -> std::shared_ptr<RenderTree> {
  //     auto self = std::static_pointer_cast<DemoWindow>(weak_this.lock());
  //     if (!self) {
  //       return nullptr;
  //     }
  //     return self->view_context_->BuildRenderTree();
  //   });
  //   auto demo_channel = TDF_MAKE_SHARED(DemoChannel);
  //   auto host_obj_windows = TDF_MAKE_SHARED(HostObjectWindows, demo_channel);
  //   engine_->GetPlatform()->GetChannelRuntime()->RegisterHostObject("demoChannel", host_obj_windows);
}

// void DemoWindow::MakeDirectory(const std::string &dir_path) {
//   auto dir = std::filesystem::path(dir_path);
//   if (!std::filesystem::exists(dir)) {
//     std::filesystem::create_directories(dir);
//   }
// }
