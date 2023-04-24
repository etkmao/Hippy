//
// Copyright (c) 2020 Tencent Corporation. All rights reserved.
//
#pragma once

#include <vector>
#include "core/common/image_frame.h"
#include "core/engine/schedule/task_driver.h"
#include "core/engine/schedule/task_runner.h"
#include "core/tdfi/shell.h"
#include "tdfui/image/image_processor.h"

namespace tdfcore {

/**
 * @brief 图片光栅化操作
 */
class ImageRasterManager : public Object {
 public:
  ~ImageRasterManager() override;

  /**
   * @brief 构造函数
   * @param raster_task_runner 光栅化TaskRunner，不可为nullptr
   */
  explicit ImageRasterManager(const std::shared_ptr<TaskRunner> &raster_task_runner);

  /**
   * @brief 结束所有任务
   * @note 请在ImageRasterManager析构之前，主动调用Terminate方法
   */
  void Terminate();

  /**
   * @brief 光栅化操作回调函数
   * @param texture 最终输出纹理
   * @param has_resized 整个光栅化操作处理过程中，是否曾发生过图像尺寸的改变
   * @note
   * 由于允许ImageProcessor的输出纹理尺寸不同于输入尺寸，因此当某一个ImageProcessor改变过输出尺寸后，has_resized将为true
   */
  using RasterCallback = std::function<void(const std::shared_ptr<Texture> &texture, bool has_resized)>;

  /**
   * @brief 光栅化操作
   * @param image_frame 解码后的图片帧
   * @param processors 图像处理器
   * @param device GPU渲染用到的Device实例
   * @param callback 回调函数
   * @return 任务实例
   */
  std::shared_ptr<Task> Raster(const std::shared_ptr<ImageFrame> &image_frame,
                               const std::vector<std::shared_ptr<ImageProcessor>> &processors,
                               const std::shared_ptr<Device> &device, const RasterCallback &callback);

 private:
  std::atomic_bool is_terminated_ = false;
  std::shared_ptr<TaskRunner> task_runner_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
