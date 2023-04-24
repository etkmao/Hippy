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

#include "modules/clipboard/clipboard.h"

#include <Windows.h>

#include "footstone/logging.h"

namespace hippy {
inline namespace framework {
inline namespace windows {
inline namespace module {

bool Clipboard::Initial() { return true; }

void Clipboard::GetString(std::function<void(footstone::value::HippyValue)> callback) {
  // 打开剪切板
  if (!OpenClipboard(NULL)) {
    FOOTSTONE_DLOG(INFO) << "Failed to open clipboard.";
    return;
  }

  // 获取剪切板数据
  HANDLE handle = GetClipboardData(CF_TEXT);
  if (handle == NULL) {
    FOOTSTONE_DLOG(INFO) << "Failed to get clipboard data.";
    CloseClipboard();
    return;
  }

  // 锁定数据并获取指针
  char* text = static_cast<char*>(GlobalLock(handle));
  if (text == NULL) {
    FOOTSTONE_DLOG(INFO) << "Failed to lock clipboard data.";
    CloseClipboard();
    return;
  }
  // 输出剪切板内容
  callback(footstone::HippyValue(text));

  // 解锁数据并关闭剪切板
  GlobalUnlock(handle);
  CloseClipboard();
  return;
}

void Clipboard::SetString(const footstone::value::HippyValue& buffer) {
  std::string clipboard_string;
  if (ParserRequest(buffer, clipboard_string)) {
    if (!OpenClipboard(NULL)) return;

    // 清空剪贴板内容
    EmptyClipboard();

    HGLOBAL handle_global = GlobalAlloc(GMEM_MOVEABLE, clipboard_string.size() + 1);
    if (handle_global != NULL) {
      char* buf = static_cast<char*>(GlobalLock(handle_global));
      if (buf != NULL) {
        memcpy(buf, clipboard_string.c_str(), clipboard_string.size() + 1);
        GlobalUnlock(handle_global);
        SetClipboardData(CF_TEXT, handle_global);
      }
    }

    // 关闭剪贴板
    CloseClipboard();
  }
  return;
}

bool Clipboard::ParserRequest(const footstone::value::HippyValue& value, std::string& clipboard_string) {
  footstone::value::HippyValue::HippyValueArrayType array;
  if (!value.ToArray(array) || array.size() != 1) return false;
  if (!array[0].ToString(clipboard_string)) return false;
  return true;
}

}  // namespace module
}  // namespace windows
}  // namespace framework
}  // namespace hippy
