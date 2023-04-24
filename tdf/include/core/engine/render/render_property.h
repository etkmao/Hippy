//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
#include "core/engine/render/box_shadow.h"
#include "core/common/diagnosticable.h"
#include "core/common/property.h"
#include "core/engine/graphics/texture.h"
#include "core/engine/render/box_border.h"

namespace tdfcore {
static inline std::string PropertiesToString(const std::vector<std::shared_ptr<Property>>& properties) {
  std::stringstream ss;
  ss << "{";
  auto count = 0;
  for (auto& it : properties) {
    ++count;
    ss << '"' << it->Name() << R"(":)";
    ss << it->ValueToString();
    if (count < properties.size()) {
      ss << ',';
    }
  }
  ss << "}";
  return ss.str();
}

// MARK: - 各种带有具体值类型的Property
template <>
class TypeProperty<TPoint> : public Property {
  TYPE_PROPERTY_INSTANTIATION(TPoint)
  std::string ValueToString() const override {
    std::stringstream ss;
    ss << R"({"x":)" << value_.x << R"(,"y":)" << value_.y << "}";
    return ss.str();
  }
};

template <>
class TypeProperty<TSize> : public Property {
  TYPE_PROPERTY_INSTANTIATION(TSize)
  std::string ValueToString() const override {
    std::stringstream ss;
    ss << R"({"w":)" << value_.width << R"(,"h":)" << value_.height << "}";
    return ss.str();
  }
};

template <>
class TypeProperty<TRect> : public Property {
  TYPE_PROPERTY_INSTANTIATION(TRect)
  std::string ValueToString() const override {
    std::stringstream ss;
    ss << R"({"x":)" << value_.left << R"(,"y":)" << value_.top << R"(,"w":)" << value_.Width() << R"(,"h":)"
       << value_.Height() << "}";
    return ss.str();
  }
};

template <>
class TypeProperty<RRect> : public Property {
  TYPE_PROPERTY_INSTANTIATION(RRect)
  std::string ValueToString() const override {
    std::stringstream ss;
    ss << "{";
    auto bounds = value_.GetRect();
    ss << R"(,"rect":{"x":)" << bounds.left << R"(,"y":)" << bounds.top << R"(,"w":)" << bounds.Width() << R"(,"h":)"
       << bounds.Height() << "}";
    auto cn = value_.GetRadii(RRect::Corner::kUpperLeft);
    ss << R"(,"cUL": {"x":)" << cn.x << R"(,"y":)" << cn.y << "}";
    cn = value_.GetRadii(RRect::Corner::kUpperRight);
    ss << R"(,"cUR": {"x":)" << cn.x << R"(,"y":)" << cn.y << "}";
    cn = value_.GetRadii(RRect::Corner::kLowerLeft);
    ss << R"(,"cLL": {"x":)" << cn.x << R"(,"y":)" << cn.y << "}";
    cn = value_.GetRadii(RRect::Corner::kLowerRight);
    ss << R"(,"cLR": {"x":)" << cn.x << R"(,"y":)" << cn.y << "}";
    ss << "}";
    return ss.str();
  }
};

template <>
class TypeProperty<TM33> : public Property {
  TYPE_PROPERTY_INSTANTIATION(TM33)
  std::string ValueToString() const override {
    std::stringstream ss;
    ss << "[" << value_[0];
    for (int i = 1; i < 9; ++i) {
      ss << ", " << value_[i];
    }
    ss << "]";
    return ss.str();
  }
};

template <>
class TypeProperty<TM44> : public Property {
  TYPE_PROPERTY_INSTANTIATION(TM44)
  std::string ValueToString() const override {
    std::stringstream ss;
    ss << "[";
    for (int r = 0; r < 4; ++r) {
      for (int c = 0; c < 4; ++c) {
        if ((r != 0) || (c != 0)) {
          ss << ", ";
        }
        ss << value_.rc(r, c);
      }
    }
    ss << "]";
    return ss.str();
  }
};

template <>
class TypeProperty<Color> : public Property {
  TYPE_PROPERTY_INSTANTIATION(Color)
  std::string ValueToString() const override {
    std::stringstream ss;
    ss << "{";
    ss << R"("red": )" << value_.GetR() << R"(,)";
    ss << R"("green": )" << value_.GetG() << R"(,)";
    ss << R"("blue": )" << value_.GetB() << R"(,)";
    ss << R"("alpha": )" << value_.GetA();
    ss << "}";
    return ss.str();
  }
};

template <>
class TypeProperty<std::shared_ptr<Texture>> : public Property {
  TYPE_PROPERTY_INSTANTIATION(std::shared_ptr<Texture>)
  std::string ValueToString() const override {
    if (value_ == nullptr) {
      return R"("")";
    }
    std::stringstream ss;
    ss << "{";
    ss << R"("dimensions": )"
       << R"({"w":)" << value_->Width() << R"(,"h":)" << value_->Height() << "},";
    ss << "}";
    return ss.str();
  }
};

template <>
class TypeProperty<std::shared_ptr<Diagnostics>> : public Property {
  TYPE_PROPERTY_INSTANTIATION(std::shared_ptr<Diagnostics>)
  std::string ValueToString() const override { return value_->ToJSONString(); }
};

template <>
class TypeProperty<BoxBorder> : public Property {
  TYPE_PROPERTY_INSTANTIATION(tdfcore::BoxBorder)
  std::string ValueToString() const override {
    auto properties = value_.MakeDiagnosticsProperties();
    return PropertiesToString(properties);
  }
};

template <>
class TypeProperty<BorderStyle> : public Property {
  TYPE_PROPERTY_INSTANTIATION(tdfcore::BorderStyle)
  std::string ValueToString() const override {
    auto properties = value_.MakeDiagnosticsProperties();
    return PropertiesToString(properties);
  }
};

template <>
class TypeProperty<BoxShadow> : public Property {
  TYPE_PROPERTY_INSTANTIATION(tdfcore::BoxShadow)
  std::string ValueToString() const override {
    auto properties = value_.MakeDiagnosticsProperties();
    return PropertiesToString(properties);
  }
};

}  // namespace tdfcore
