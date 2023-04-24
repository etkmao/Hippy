
#pragma once
#include <unordered_map>
#include "core/common/object.h"
namespace tdfcore {
enum class SystemMouseCursor {
  kUnknown,  // 未知状态，不直接对应平台类型，和kNone不一样，不允许使用。
  kAlias,
  kAllScroll,
  kBasic,
  kCell,
  kClick,
  kContextMenu,
  kCopy,
  kDisappearing,
  kForbidden,
  kGrab,
  kGrabbing,
  kHelp,
  kMove,
  kNoDrop,
  kNone,
  kPrecise,
  kProgress,
  kResizeColumn,
  kResizeDown,
  kResizeDownLeft,
  kResizeDownRight,
  kResizeLeft,
  kResizeLeftRight,
  kResizeRight,
  kResizeRow,
  kResizeUp,
  kResizeUpDown,
  kResizeUpLeft,
  kResizeUpLeftDownRight,
  kResizeUpRight,
  kResizeUpRightDownLeft,
  kText,
  kVerticalText,
  kWait,
  kZoomIn,
  kZoomOut
};
}  // namespace tdfcore
