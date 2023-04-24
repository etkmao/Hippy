//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include <vector>
#include "tdfui/view/text/text_selection_control.h"

namespace tdfcore {
class CupertinoTextSelectionControl : public TextSelectionControl {
 public:
  ~CupertinoTextSelectionControl() override = default;
  constexpr static double kHandleOverlap = 1.5;
  constexpr static double kHandleRadius = 4;
  constexpr static double kToolbarRadius = 4;
  constexpr static size_t kToolbarItemPadding = 4;
  constexpr static TScalar kToolbarTextSize = 14;
  constexpr static Color kHandlerColor = Color::ARGB(255, 0, 0, 255);
  constexpr static Color kToolbarBackgroundColor = Color::ARGB(255, 44, 44, 44);
  constexpr static Color kToolbarTextColor = Color::ARGB(255, 210, 210, 210);

  /**
   * @brief 创建句柄
   * @param type 文本选取句柄类型
   * @param text_line_height 文本行的高度
   * @return
   */
  std::shared_ptr<RenderObject> BuildHandle(TextSelectionHandleType type, double text_line_height) override;

  TPoint GetHandleAnchor(TextSelectionHandleType type, double text_line_height) override;

  TSize GetHandleSize(double text_line_height) override;

  std::shared_ptr<RenderObject> BuildToolbar(std::function<void()> cut_handler, std::function<void()> copy_handler,
                                             std::function<void()> paste_handler,
                                             std::function<void()> selection_all_handler) override;

 protected:
  CupertinoTextSelectionControl() = default;

 private:
  static std::shared_ptr<RenderObject> BuildToolbarItem(const std::u16string &text, TPoint offset);
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
