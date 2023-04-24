//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/platform/common/clipboard.h"

namespace tdfcore {

class ClipboardMacOS final : public Clipboard {
 public:
  bool SetData(const std::shared_ptr<ClipboardData>& data) override;
  std::shared_ptr<ClipboardData> GetData(ClipboardFormatType formatType) override;
};
}  // namespace tdfcore
