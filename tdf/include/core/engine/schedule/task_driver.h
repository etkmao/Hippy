//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <mutex>
#include <string>
#include <vector>
#include "core/common/macro.h"
#include "core/common/object.h"
#include "core/common/waitable_object.h"

namespace tdfcore {

class TaskDriver : public Object, public std::enable_shared_from_this<TaskDriver> {
 public:
  ~TaskDriver() override;

  /**
   * @brief 驱动逻辑的执行体
   * @param driver 当前TaskDriver
   * @return 下次执行驱动逻辑的（推荐的）延时时间
   */
  using Callback = std::function<TDuration()>;

  /**
   * @brief 构造函数
   * @param callback 驱动逻辑Callback
   */
  explicit TaskDriver(const Callback &callback = nullptr);

  /**
   * @brief 获取当前线程正在执行驱动的TaskDriver
   */
  static std::shared_ptr<TaskDriver> GetCurrent();

  /**
   * @brief 启动
   * @note 在Stopping状态下调用Start会返回false
   * @return 是否运行
   */
  bool Start();

  /**
   * @brief 停止
   */
  std::shared_ptr<WaitableObject> Stop();

  /**
   * @brief TaskDriver的状态枚举
   */
  enum Status {
    kStopped,
    kStopping,
    kRunning,
  };

  /**
   * @brief 获取状态
   */
  Status GetStatus() const;

  /**
   * @brief 是否正在运行中
   */
  inline bool IsRunning() const { return GetStatus() == Status::kRunning; }

  /**
   * @brief 是否正在停止中
   */
  inline bool IsStopping() const { return GetStatus() == Status::kStopping; }

  /**
   * @brief 是否已经停止
   */
  inline bool IsStopped() const { return GetStatus() == Status::kStopped; }

  /**
   * @brief 设置驱动逻辑Callback
   */
  void SetCallBack(const Callback &callback);

  /**
   * @brief 设置名称
   */
  virtual void SetName(const std::string &name);

  /**
   * @brief 获取名称
   */
  constexpr const std::string &GetName() const { return name_; }

  /**
   * @brief 唤醒TaskDriver的阻塞态，使其尽快执行一次驱动
   */
  virtual void WakeUp() = 0;

  /**
   * @brief 获取TaskDriver执行时所在的线程id
   */
  constexpr uint64_t GetThreadId() const { return thread_id_; }

 protected:
  /**
   * @brief 执行一次驱动逻辑
   * @return 下次执行RunOnce的（推荐的）延时时间
   */
  TDuration RunOnce();

  /**
   * @brief 从Stopping状态切换到Stopped状态的处理逻辑
   */
  void FinishIfNeeded();

  /**
   * @brief 子类可重写该方法，以追加自己特殊的TaskDriver启动逻辑
   */
  virtual void OnStart() {}

  /**
   * @brief 子类可重写该方法，以追加自己特殊的TaskDriver停止逻辑
   */
  virtual void OnStop() {}

 private:
  Callback callback_;
  std::string name_;
  mutable std::mutex mutex_;
  Status status_ = Status::kStopped;
  uint64_t thread_id_ = 0;
  std::vector<std::shared_ptr<WaitableObject>> stop_waiters_;

  FRIEND_OF_TDF_ALLOC;
};

class TaskRunner;

/**
 * @brief 创建一个TaskDriver::Callback，Callback内实现了对单个TaskRunner的调度
 * @param runner 被调度的TaskRunner
 * @param driver 驱动方TaskDriver
 */
TaskDriver::Callback MakeCallbackForSchedulingTaskRunnerOnTaskDriver(const std::shared_ptr<TaskRunner> &runner,
                                                                     const std::shared_ptr<TaskDriver> &driver);

}  // namespace tdfcore
