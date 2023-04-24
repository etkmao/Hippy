//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include <vector>
#include "core/common/data.h"
#include "core/common/impl_getter.h"
#include "core/common/m33.h"
#include "core/common/object.h"
#include "core/common/rect.h"
#include "core/common/rrect.h"
#include "core/common/scalar.h"

namespace tdfcore {

class PathImpl;

class Path : public Object, public ImplGetter {
 public:
  ~Path() override = default;
  Path();
  Path(const Path& path);
  Path(Path&& path);
  Path& operator=(const Path& path);

  enum class AddPathMode {
    kAppend,  //!< appended to destination unaltered
    kExtend,  //!< add line if prior contour is not closed
  };

  enum class PathDirection {
    kCW,
    kCCW,
  };

  enum ArcSize {
    kSmall_ArcSize,  //!< smaller of arc pair
    kLarge_ArcSize,  //!< larger of arc pair
  };

  enum class PathFillType {
    kWinding,
    kEvenOdd,
    kInverseWinding,
    kInverseEvenOdd,
  };

  enum PathOp {
    kDifference_PathOp,         //!< subtract the op path from the first path
    kIntersect_PathOp,          //!< intersect the two paths
    kUnion_PathOp,              //!< union (inclusive-or) the two paths
    kXOR_PathOp,                //!< exclusive-or the two paths
    kReverseDifference_PathOp,  //!< subtract the first path from the op path
  };

  static bool TwoPathOp(const Path& one, const Path& two, PathOp op, Path* result);

  static Path CreateDrawArcPath(const TRect &oval, TScalar start_angle,
                                TScalar sweep_angle, bool use_center, bool is_fill_no_path_effect);

  void Offset(TScalar x, TScalar y);
  void Transform(const TM33& matrix);

  bool Contains(TScalar x, TScalar y);

  Path& MoveTo(TScalar x, TScalar y);
  Path& LineTo(TScalar x, TScalar y);
  Path& RLineTo(TScalar x, TScalar y);
  Path& QuadTo(TScalar x1, TScalar y1, TScalar x2, TScalar y2);
  Path& RQuadTo(TScalar dx1, TScalar dy1, TScalar dx2, TScalar dy2);
  Path& CubicTo(TScalar x1, TScalar y1, TScalar x2, TScalar y2, TScalar x, TScalar y);
  Path& ArcTo(const TRect& oval, TScalar start_angle, TScalar sweep_angle, bool force_move_to);
  Path& ArcTo(TScalar x1, TScalar y1, TScalar x_axis_rotate, ArcSize large_arc, PathDirection sweep, TScalar x2,
              TScalar y2);

  Path& AddRect(const TRect& rect);
  Path& AddRRect(const RRect& rrect);
  Path& AddRoundRect(const TRect& rect, TScalar* radii);
  Path& AddOval(const TRect& rect, PathDirection dir = PathDirection::kCW);
  Path& AddCircle(TScalar x, TScalar y, TScalar radius, PathDirection dir = PathDirection::kCW);
  Path& AddPoly(const TPoint pts[], int count, bool close);
  Path& AddPath(const Path& src, const TM33& matrix = TM33(), AddPathMode op = AddPathMode::kAppend);

  void SetFillType(PathFillType type);
  PathFillType GetFillType() const;
  void SetIsVolatile(bool isVolatile);
  bool IsInverseFillType() const;
  bool IsValid() const;
  bool IsEmpty() const;
  bool GetLastPt(TPoint* last_point) const;

  TRect GetBounds() const;
  TRect ComputeTightBounds();

  std::shared_ptr<TData> serialize() const;

  void Close();
  void Reset();

  friend bool operator==(const Path& a, const Path& b);

 protected:
  std::any Get() override;
  std::any GetConst() const override;
  std::shared_ptr<PathImpl> impl_;
};
}  // namespace tdfcore
