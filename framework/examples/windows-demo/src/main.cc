#include <Windows.h>
#include <memory>

#include "config.h"
#include "demo_window.h"
#include "framework.h"

using namespace std;
using Framework = hippy::windows::framework::Framework;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  std::shared_ptr<Framework> framework = std::make_shared<Framework>();
  auto demo_window = std::make_shared<DemoWindow>();
  demo_window->SetFramework(framework);
  demo_window->Initialize("Hippy Window Demo", tdfcore::TRect::MakeXYWH(0, 0, 600, 800));
  demo_window->Show();

  MSG msg;
  while (::GetMessage(&msg, nullptr, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }

  CoUninitialize();
  return EXIT_SUCCESS;
}
