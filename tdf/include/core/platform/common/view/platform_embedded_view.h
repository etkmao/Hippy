//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <future>
#include <map>
#include <string>
#include "core/common/bitmap.h"
#include "core/platform/common/channel/channel_runtime.h"
#include "core/platform/common/view/mutators_stack.h"
#include "core/platform/common/view/platform_view.h"
#include "core/support/gesture/recognizer/gesture_recognizer.h"

namespace tdfcore {
/**
 * @brief 复用宿主Native View的基类
 */
class PlatformEmbeddedView : public PlatformView {
 public:
  using PropsMap = std::map<std::string, std::string>;
  PlatformEmbeddedView(const std::string &view_type, const PlatformEmbeddedView::PropsMap &props_map,
                       const std::shared_ptr<ChannelRuntime> &channel_runtime);

  /**
   * @brief 更新View的属性
   * @param props view的属性
   */
  virtual void UpdateProps(const PropsMap &props) = 0;

  /**
   * @brief 立即同步变换栈到Native
   * 将Transform、Clip、Opacity同步至NativeView
   * EmbeddedView会应用最新的变化
   */
  virtual void UpdateMutatorsToNative(const MutatorsStack &mutators_stack) = 0;

  virtual std::future<std::shared_ptr<Bitmap>> MakeSnapshot();

 protected:
  std::shared_ptr<ChannelRuntime> channel_runtime_;

 private:
  PlatformEmbeddedView::PropsMap props_map_;
};

}  // namespace tdfcore
