#pragma once

#include "core/platform/common/view/platform_embedded_view.h"

namespace tdfcore {
constexpr static int64_t kInvalidIndex = -1;
struct EmbeddedViewHolder {
  int64_t id;
  std::shared_ptr<PlatformEmbeddedView> platform_embedded_view;
  int64_t embedded_view_index = kInvalidIndex;
  std::shared_ptr<PlatformView> overlay_view;
  int64_t overlay_view_index = kInvalidIndex;
  // 相同位置，且 id && embedded_view_index && overlay_view_index 相同才可能复用
  bool can_reuse = false;
};
}  // namespace tdfcore
