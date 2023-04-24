#include <Windows.h>
#include <memory>

#include "config.h"
#include "context.h"
#include "demo_window.h"
#include "framework.h"

using namespace std;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  std::shared_ptr<hippy::Context> context = std::make_shared<hippy::Context>();
  context->SetDensity(1.0f);
  context->CreateRootNode();
  context->CreateVfsManager();
  context->CreateUriLoader();
  std::shared_ptr<hippy::FrameWork> framework = std::make_shared<hippy::FrameWork>();
  framework->SetContext(context);
  std::shared_ptr<hippy::Config> config = std::make_shared<hippy::Config>(true, true, false, 10, 10, "./");
  framework->Initialize(config);

  auto demo_window = std::make_shared<DemoWindow>();
  demo_window->SetTDFRenderManager(framework->GetRenderManager());
  demo_window->SetRootId(10);
  demo_window->Initialize("Hippy Window Demo", tdfcore::TRect::MakeXYWH(0, 0, 600, 800));
  demo_window->Show();

  // run script file
  bool ret = framework->RunScriptFromUri("asset:/index.bundle");

  framework->LoadInstance();

  MSG msg;
  while (::GetMessage(&msg, nullptr, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }

  CoUninitialize();
  return EXIT_SUCCESS;
}
