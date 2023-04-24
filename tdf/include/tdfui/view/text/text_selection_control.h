//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include <vector>
#include "tdfui/view/clipboard_status_notifier.h"
#include "core/support/text/text_base.h"
#include "core/common/size.h"
#include "core/platform/common/text_input.h"
#include "tdfui/view/view.h"

namespace tdfcore {
class RenderObject;

enum class TextSelectionHandleType { Left, Right, Collapsed };

enum class TextSelectionHandlePosition { Start, End };

class TextSelectionControl : public Object {
 public:
  using Handler = std::function<void()>;
  // TODO(willisdai) 多语言支持
  static constexpr const char16_t *kCopyItemDisplayName = u"复制";
  static constexpr const char16_t *kSelectAllItemDisplayName = u"全选";
  static constexpr const char16_t *kCutItemDisplayName = u"剪切";
  static constexpr const char16_t *kPasteItemDisplayName = u"粘贴";

  virtual std::shared_ptr<RenderObject> BuildHandle(TextSelectionHandleType type,
                                                    double text_line_height) = 0;

  virtual TPoint GetHandleAnchor(TextSelectionHandleType type, double text_line_height) = 0;

  virtual TSize GetHandleSize(double text_line_height) = 0;

  virtual std::shared_ptr<RenderObject> BuildToolbar(Handler cut_handler = nullptr,
                                                     Handler copy_handler = nullptr,
                                                     Handler paste_handler = nullptr,
                                                     Handler selection_all_handler = nullptr) = 0;
};
}  // namespace tdfcore
