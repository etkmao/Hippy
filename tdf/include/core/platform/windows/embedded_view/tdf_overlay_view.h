//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include "core/platform/windows/view/tdf_view.h"

namespace tdfcore {

class TDFWindow;
class TDFOverlayView : public TDFView {
 public:
  explicit TDFOverlayView(int64_t view_id);

 private:
  int64_t view_id_;
};

}  // namespace tdfcore