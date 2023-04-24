//
// Copyright © 2020 com.tencent. All rights reserved.

//

#pragma once
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/common/clipboard.h"

namespace tdfcore {

/**
 * @brief Android剪切板实现类
 */
class ClipboardAndroid final : public Clipboard {
 public:
  explicit ClipboardAndroid(JavaWeakGlobalRef<jobject> df_clipboard_weak_ref);
  bool SetData(const std::shared_ptr<ClipboardData>& data) override;
  std::shared_ptr<ClipboardData> GetData(ClipboardFormatType formatType) override;

 private:
  JavaWeakGlobalRef<jobject> df_clipboard_weak_ref_;

  jmethodID get_text_data_;
  jmethodID set_text_data_;
};
}  // namespace tdfcore
