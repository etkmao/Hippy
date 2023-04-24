//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/platform/common/channel/channel.h"
#include "tdfui/view/embedded_view.h"

namespace tdfcore {
/**
 * @brief 手写笔描绘视图
 * 仅iOS平台实现
 */
class PencilCanvasView : public EmbeddedView {
 public:
  /**
   * @brief 获取描绘的数据
   * 建议在非UI线程执行
   * @return 描绘的数据
   */
  std::shared_ptr<TData> GetDrawingData();

  /**
   * @brief 获取描绘的图片
   * 建议在非UI线程执行
   * @param rect 图片区域
   * @param scale 图片像素系数
   * @return 图片
   */
  std::shared_ptr<Bitmap> GetDrawingImage(TRect rect, float scale);

  /**
   * @brief 设置描绘数据
   */
  void SetDrawingData(const std::shared_ptr<TData>& drawing_data);

  /**
   * @brief 清屏
   */
  void Clear();

  /**
   * @brief 开始绘画
   * 弹起工具栏，PencilCanvasView会优先处理手势
   */
  void StartDrawing();

  /**
   * @brief 结束绘画
   */
  void FinishDrawing();

 protected:
  explicit PencilCanvasView(const std::shared_ptr<TData>& drawing_data = nullptr,
                            const std::shared_ptr<ViewContext>& view_context = ViewContext::GetCurrent());

 private:
  void Init() override;
  void Load() override;
  std::shared_ptr<Channel>& GetChannel();

  std::shared_ptr<TData> drawing_data_;
  std::shared_ptr<Channel> channel_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
