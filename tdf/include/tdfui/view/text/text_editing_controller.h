//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include <string>
#include <utility>
#include "core/support/text/text_base.h"
#include "core/common/diagnosticable.h"
#include "core/common/value_change_notifier.h"
#include "core/platform/common/text_input.h"
#include "core/support/text/richtext/inline_span.h"

namespace tdfcore {

class TextEditingController : public Object,
                              public Diagnosticable,
                              public ValueNotifier<TextEditingValue> {
 public:
  explicit TextEditingController(std::u16string text = u"")
      : ValueNotifier<TextEditingValue>(TextEditingValue(std::move(text))) {}

  virtual std::u16string GetText() const { return GetValue().GetText(); }

  virtual void SetText(const std::u16string &text);

  virtual TextEditingValue UpdateEditingValue(const TextEditingValue& value);

  virtual std::vector<std::shared_ptr<InlineSpan>> BuildText();

  const tdfcore::TextSelection &GetSelection() const { return GetValue().GetSelection(); }

  void SetSelection(const tdfcore::TextSelection &text_selection);

  void SetComposing(const TextRange &composing);

  void ClearComposing();

  std::shared_ptr<Diagnostics> ToDiagnostics() const override;

  constexpr void SetIsObscure(bool is_obscure) { is_obscure_ = is_obscure; }

  constexpr void SetDefaultTextStyle(const TextStyle& style) { default_text_style_ = style; }

 protected:
  bool is_obscure_ = false;
  TextStyle default_text_style_;
};
}  // namespace tdfcore
