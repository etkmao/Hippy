//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/macro.h"
#include "core/common/point.h"
#include "core/common/serialization.h"
#include "core/common/size.h"

namespace tdfcore {

/**
 * @brief 代表屏幕可绘制区域信息
 */
struct alignas(8) ViewportMetrics {
 public:
  // 像素相对密度
  double device_pixel_ratio = 1.0f;
  // 宽度
  double width = 0;
  // 高度
  double height = 0;
  // 顶部插入额外空间
  double view_inset_top = 0;
  // 右边插入额外空间
  double view_inset_right = 0;
  // 底部插入额外空间(例会弹起键盘时，键盘会占用底部空间)
  double view_inset_bottom = 0;
  // 左边插入额外空间
  double view_inset_left = 0;
  // 顶部间距(例如状态栏高度)
  double view_padding_top = 0;
  // 右边距
  double view_padding_right = 0;
  // 底部边距
  double view_padding_bottom = 0;
  // 左边距
  double view_padding_left = 0;
  // 系统手势触发上边距
  double system_gesture_inset_top = 0;
  // 系统手势触发右边距
  double system_gesture_inset_right = 0;
  // 系统手势触发底部边距
  double system_gesture_inset_bottom = 0;
  // 系统手势触发左边距
  double system_gesture_inset_left = 0;
  // TDFView 基于窗口的x
  double root_view_x_from_window = 0;
  // TDFView 基于窗口的y
  double root_view_y_from_window = 0;

  TDF_SERIALIZATION(device_pixel_ratio, width, height, view_inset_top, view_inset_right, view_inset_bottom,
                    view_inset_left, view_padding_top, view_padding_right, view_padding_bottom, view_padding_left,
                    system_gesture_inset_top, system_gesture_inset_right, system_gesture_inset_bottom,
                    system_gesture_inset_left, root_view_x_from_window, root_view_y_from_window);

  /*
   * @brief 获取平台视图尺寸
   * @return 返回尺寸大小,尺寸单位为像素
   */
  ISize ViewSize() const { return ISize::Make(width, height); }

  /**
   * @brief 将TDFView坐标系转换至窗口坐标
   */
  TPoint LocalToWindow(TPoint point) const {
    return TPoint::Make(root_view_x_from_window + point.x, root_view_y_from_window + point.y);
  }

  /**
   * @brief 将数据严格按照ViewportMetrics的内存布局结构进行反序列化，生成ViewportMetrics实例
   */
  static ViewportMetrics MakeFromData(const uint8_t *data, size_t size) {
    assert(size == sizeof(ViewportMetrics));
    ViewportMetrics metrics;
    memcpy(&metrics, data, size);
    return metrics;
  }

  constexpr bool operator==(const ViewportMetrics &viewport_metrics) const {
    return device_pixel_ratio == viewport_metrics.device_pixel_ratio && width == viewport_metrics.width &&
           height == viewport_metrics.height && view_inset_top == viewport_metrics.view_inset_top &&
           view_inset_right == viewport_metrics.view_inset_right &&
           view_inset_bottom == viewport_metrics.view_inset_bottom &&
           view_inset_left == viewport_metrics.view_inset_left &&
           system_gesture_inset_top == viewport_metrics.system_gesture_inset_top &&
           system_gesture_inset_right == viewport_metrics.system_gesture_inset_right &&
           system_gesture_inset_bottom == viewport_metrics.system_gesture_inset_bottom &&
           system_gesture_inset_left == viewport_metrics.system_gesture_inset_left &&
           view_padding_top == viewport_metrics.view_padding_top &&
           view_padding_left == viewport_metrics.view_padding_left &&
           view_padding_right == viewport_metrics.view_padding_right &&
           view_padding_bottom == viewport_metrics.view_padding_bottom;
  }
  friend constexpr bool operator!=(const ViewportMetrics &lt_viewport_metrics,
                                   const ViewportMetrics &rt_viewport_metrics) {
    return !(lt_viewport_metrics == rt_viewport_metrics);
  }
};

}  // namespace tdfcore
