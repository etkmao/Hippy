//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include "core/platform/common/platform_window.h"

@class TDFWindow;
namespace tdfcore {

class PlatformWindowMacos : public PlatformWindow {
 public:
  PlatformWindowMacos(const std::string_view& title, const TRect& frame);
  void Init() override;
  void Show() override;
  void Hide() override;
  void AddView(const std::shared_ptr<PlatformView>& platform_view) override;
  void RemoveView(const std::shared_ptr<PlatformView>& platform_view) override;
private:
  TDFWindow* window_;
};

}



