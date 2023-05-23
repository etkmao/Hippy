#include <Windows.h>
#include <memory>

#include "main_window.h"

using namespace std;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  auto main_window = std::make_shared<MainWindow>();
  main_window->Show();

  MSG msg;
  while (::GetMessage(&msg, nullptr, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }

  CoUninitialize();
  return EXIT_SUCCESS;
}
