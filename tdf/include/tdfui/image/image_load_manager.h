//
// Copyright (c) 2020 Tencent Corporation. All rights reserved.
//
#pragma once

#include <unordered_map>
#include <unordered_set>
#include "core/engine/schedule/scheduler.h"
#include "core/engine/schedule/task_driver.h"
#include "core/engine/schedule/task_runner.h"
#include "tdfui/image/image_loader.h"

namespace tdfcore {

/**
 * @brief 图片加载管理中心
 */
class ImageLoadManager : public Object, public std::enable_shared_from_this<ImageLoadManager> {
 public:
  ~ImageLoadManager() override;

  /**
   * @brief 构造函数
   * @param scheduler 通过指定的Scheduler去创建TaskRunner与TaskDriver
   */
  explicit ImageLoadManager(const std::shared_ptr<Scheduler> &scheduler);

  /**
   * @brief 结束所有任务
   * @note 请在ImageLoadManager析构之前，主动调用Terminate方法
   */
  void Terminate();

  /**
   * @brief 加载图片
   * @param url 图片url
   * @param progress_callback 图片加载进度回调
   * @param finish_callback 图片加载完成回调
   * @return 任务回调id，可用于取消加载图片（通过Cancel方法）
   */
  uint64_t Load(const std::string &url, const ImageLoader::ProgressCallback &progress_callback,
                const ImageLoader::FinishCallback &finish_callback);

  /**
   * @brief 取消加载图片
   * @param url 图片url
   * @param callback_id 任务回调id（由Load方法返回的id）
   */
  void Cancel(const std::string &url, uint64_t callback_id);

  /**
   * @brief 注册ImageLoader
   * @param scheme 图片Scheme类型
   * @param loader 图片加载器
   */
  void RegisterImageLoader(const std::string &scheme, const std::shared_ptr<ImageLoader> &loader);

  /**
   * @brief 移除ImageLoader
   * @param scheme 图片Scheme类型
   */
  void UnregisterImageLoader(const std::string &scheme);

 private:
  struct LoadTask {
    std::shared_ptr<Task> task;
    std::unordered_map<uint64_t, ImageLoader::ProgressCallback> progress_callbacks;
    std::unordered_map<uint64_t, ImageLoader::FinishCallback> finish_callbacks;
  };

  std::atomic_bool is_terminated_ = false;
  std::shared_ptr<TaskRunner> task_runner_;
  std::unordered_set<std::shared_ptr<TaskDriver>> task_drivers_;
  std::unordered_map<std::string, std::shared_ptr<ImageLoader>> image_loader_map_;
  std::unordered_map<std::string, std::shared_ptr<LoadTask>> task_map_;
  std::mutex task_mutex_;
  std::shared_ptr<LoadTask> MakeLoadTask(const std::string &url, const std::shared_ptr<ImageLoader> &loader);
  uint64_t AddCallbacksForTask(const std::shared_ptr<LoadTask> &task,
                               const ImageLoader::ProgressCallback &progress_callback,
                               const ImageLoader::FinishCallback &finish_callback);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
