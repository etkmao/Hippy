//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "tdfui/view/text/text_view.h"

namespace tdfcore {
/**
 * @brief 文本控件
 */
class Label : public TextView {
  TDF_REFF_CLASS_META(Label)

 public:
  void Init() override;

  /**
   * @brief 设置文字颜色
   * @param color 颜色
   */
  void SetTextColor(Color color);

  /**
   * @brief 获取文字颜色
   * @return 颜色
   */
  Color GetTextColor() const;

  /**
   * @brief 设置字体大小
   * @param size 大小
   */
  void SetFontSize(TScalar size);

  /**
   * @brief 获取字体大小
   * @return 大小
   */
  TScalar GetFontSize() const;

  /**
   * @brief 计算出最优的size，并设置成自身frame的size
   */
  void SizeToFit();

  /**
   * @brief 计算出最优的size
   */
  TSize SizeThatFit(TSize size);

 protected:
  explicit Label(const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent())
      : TextView(view_context) {}

 private:
  void UpdateTextStyle(const TextStyle &text_style);
  TextStyle text_style_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
TDF_REFL_DEFINE(tdfcore::Label, bases<tdfcore::TextView>)
TDF_REFL_END(tdfcore::Label)
