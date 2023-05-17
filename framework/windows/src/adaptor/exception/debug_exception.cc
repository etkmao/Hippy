
/*
 *
 * Tencent is pleased to support the open source community by making
 * Hippy available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "adaptor/exception/debug_exception.h"

#include <Windows.h>
#include <richedit.h>

#include "footstone/logging.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace adaptor {

DebugException::DebugException(bool debug) : debug_(debug) {}

bool DebugException::Initial() {}

BOOL CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_INITDIALOG:  // 对话框初始化
      return TRUE;

    case WM_COMMAND:  // 处理按钮点击事件
      switch (LOWORD(wParam)) {
        case IDOK:  // 点击确定按钮
        {
          return TRUE;
        }

        case IDCANCEL:         // 点击取消按钮
          EndDialog(hwnd, 0);  // 关闭对话框
          return TRUE;
      }
      break;
  }
  return FALSE;
}

void DebugException::HandleException(const std::string& desc, const std::string& stack) {
  if (debug_) {
#ifdef ERROR
#undef ERROR
    FOOTSTONE_DLOG(ERROR) << "desc: " << desc << ", stack: " << stack;
#endif

    LoadLibrary(TEXT("Riched20.dll"));

    HWND handle_edit = CreateWindowEx(
        0, RICHEDIT_CLASS, "Exception", WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 600, HWND_DESKTOP, nullptr, GetModuleHandle(nullptr), nullptr);
    if (handle_edit) {
      SendMessage(handle_edit, EM_SETEVENTMASK, 0, ENM_SELCHANGE | ENM_LINK);  // 设置事件掩码
      SendMessage(handle_edit, EM_SETBKGNDCOLOR, 0, RGB(244, 96, 108));       // 设置背景

      CHARFORMAT cf;
      ZeroMemory(&cf, sizeof(cf));
      cf.cbSize = sizeof(cf);
      cf.dwMask = CFM_FACE | CFM_SIZE | CFM_COLOR;
      // cf.szFaceName = TEXT("宋体");
      strcpy_s(cf.szFaceName, LF_FACESIZE, "Arial");
      cf.yHeight = 200;
      cf.crTextColor = RGB(255, 255, 255);
      SendMessage(handle_edit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);  // 设置默认字符格式

      std::string exception_message = "excption description:";
      exception_message += "\r\n";
      exception_message += desc;
      exception_message += "\r\n\r\n\r\n";
      exception_message += "exception stack:";
      exception_message += stack;
      SendMessage(handle_edit, WM_SETTEXT, 0, (LPARAM)TEXT(exception_message.c_str()));  // 插入文本

      MSG msg;
      while (GetMessage(&msg, NULL, 0, 0)) {
        // 处理子窗口消息
        if (msg.hwnd == handle_edit) {
          if (msg.message == WM_DESTROY) TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
      }
    }
  }
  return;
}

void DebugException::HandleNativeException(const std::string& desc, const std::string& stack) { return; };

void DebugException::HandleBackgroundTracing(const std::string& stack) {
  if (debug_) {
    FOOTSTONE_DLOG(INFO) << stack;
  }
}

}  // namespace adaptor
}  // namespace framework
}  // namespace windows
}  // namespace hippy
