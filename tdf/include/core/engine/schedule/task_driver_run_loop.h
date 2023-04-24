//
// Copyright © 2020 com.tencent. All rights reserved.
//

#pragma once

#include "core/common/waitable_object.h"
#include "core/engine/schedule/task_runner.h"

namespace tdfcore {

/**
 * @brief TaskDriverRunLoop提供了一套驱动任务调度执行的调度流程，其宿主（即调度的驱动方）为TaskDriver类型
 */
class TaskDriverRunLoop : public Object, public std::enable_shared_from_this<TaskDriverRunLoop> {
 public:
  ~TaskDriverRunLoop() override;

  /**
   * @brief 构造函数
   * @param weak_up_callback 用于唤醒宿主，使得宿主去触发RunOnce
   */
  explicit TaskDriverRunLoop(const std::function<void()> &weak_up_callback = nullptr);

  /**
   * @brief 驱动执行一轮调度流程；宿主应当反复调用本方法以驱动本类进行工作
   * @detail 流程包含三步：等待任务、执行任务、暂停处理；
   *         Resume、Puase等方法、以及TaskDriver本身的状态，都会影响以上某些步骤是否执行
   * @param driver_running TaskDriver是否处于运行状态（对应TaskDriver::IsRunning()方法）
   * @return 返回下一次执行Drive的（推荐的）延迟时间
   */
  TDuration Drive(bool driver_running);

  /**
   * @brief 设置被调度的function对象
   */
  void SetTaskFunction(const std::function<void()> &func);

  /**
   * @brief 添加被调度的TaskRunner对象
   */
  void AddTaskRunners(const std::vector<std::shared_ptr<TaskRunner>> &task_runners);

  /**
   * @brief 设置任务执行时间栅栏
   */
  void SetTimeFence(const TimePoint &time_fence);

  /**
   * @brief 进入运行状态
   */
  void Resume();

  /**
   * @brief 进入暂停状态
   * @return 可用于等候暂停操作完成
   */
  std::shared_ptr<WaitableObject> Pause();

  /**
   * @brief 启动一轮调度流程，完成之后自动触发暂停
   * @return 可用于等候暂停操作完成
   */
  std::shared_ptr<WaitableObject> RunOnce();

 protected:
  // 是否处于运行状态
  bool is_running_ = true;

  // 用于唤醒宿主
  std::function<void()> weak_up_callback_;

  // 被调度的function
  std::function<void()> task_function_;

  // 被调度的TaskRunner
  std::vector<std::shared_ptr<TaskRunner>> task_runners_;

  // 被调度的TaskRunner
  std::unordered_map<std::shared_ptr<TaskRunner>, uint64_t> task_runner_callbacks_;

  // TaskRunner的时间栅栏
  TimePoint time_fence_ = kInvalidTaskExecutionTimeFence;

  // 是否触发一次自动暂停操作
  bool needs_pause_ = false;

  // 用于等候暂停操作完成
  std::vector<std::shared_ptr<WaitableObject>> pause_waiters_;

  // 保护内部调度状态的互斥锁
  std::mutex state_mutex_;

  // 保护任务队列的互斥锁
  std::mutex task_mutex_;

  // 获取下一个马上要执行task的delay时长
  TDuration GetDelayOfNextTask();

  // 执行任务
  void RunTasks();

  // 任务执行相关的逻辑处理；返回值表示本次是否执行了任务
  bool RunTasksIfNeeded();

  // 暂停相关的逻辑处理；返回值表示当前是否已经为暂停状态
  bool PauseIfNeeded();

  // 解除宿主的阻塞状态，以触发任务调度执行
  void WakeUp(bool force);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
