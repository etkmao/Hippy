//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/platform/common/channel/host_object.h"
#include "core/platform/common/clipboard.h"
#include "core/platform/windows/windows_define.h"

namespace tdfcore {

class ClipboardWindows final : public Clipboard {
 public:
  bool SetData(const std::shared_ptr<ClipboardData>& data) override;
  std::shared_ptr<ClipboardData> GetData(ClipboardFormatType formatType) override;
};

}  // namespace tdfcore
