//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <cassert>
#include <future>
#include <mutex>
#include <queue>
#include <vector>
#include "core/common/callback_notifier.h"
#include "core/common/time.h"

namespace tdfcore {

/**
 * @brief 无效的任务执行时间栅栏，可代表时间栅栏不存在
 */
constexpr static const TimePoint kInvalidTaskExecutionTimeFence = TimePoint(kMaximumDuration);

/**
 * @brief 【基类】任务
 */
class Task : public Object, public std::enable_shared_from_this<Task> {
 public:
  ~Task() override = default;

  /**
   * @brief 获取当前线程正在执行任务的Task实例
   */
  static std::shared_ptr<Task> GetCurrent();

  /**
   * @brief 执行
   */
  void Run();

  /**
   * @brief 取消
   */
  void Cancel() { is_canceled_ = true; }

  /**
   * @brief 是否已取消
   */
  constexpr bool IsCanceled() const { return is_canceled_; }

 protected:
  bool is_canceled_ = false;

  Task() = default;
  virtual void OnRun() = 0;
  bool CheckDeadlock();

  FRIEND_OF_TDF_ALLOC;
};

template <typename R = void>
struct TaskFunction {
  using Type = std::function<R()>;
};

template <typename R>
using TaskFunctionType = typename TaskFunction<R>::Type;

/**
 * @brief 异步任务
 */
template <typename R>
class FutureTask : public Task {
 public:
  ~FutureTask() override = default;

  /**
   * @brief 构造函数
   * @param func 待执行的具体任务，该任务可具有返回值，不可为nullptr
   */
  explicit FutureTask(const TaskFunctionType<R> &func) : Task(), packaged_task_(std::packaged_task<R()>(func)) {
    assert(func);
  }

  /**
   * @brief 等待任务执行完毕，并获取任务的返回值
   * @return 任务的返回值
   */
  R Wait() {
    // 检查：若该Task已投进某一TaskRunner，并等待异步调度执行，此刻在当前线程里调用Wait()，是否有可能引发线程死锁
    // 若检查不通过，请调用者斟酌自己的代码逻辑在当前的线程模型下是否存在死锁隐患
    assert(CheckDeadlock());
    return packaged_task_.get_future().get();
  }

 protected:
  void OnRun() override {
    if (has_result_) {
      packaged_task_.reset();
      has_result_ = false;
    }
    packaged_task_();
    has_result_ = true;
  }

 private:
  std::packaged_task<R()> packaged_task_;
  bool has_result_ = false;

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 任务队列
 */
class TaskRunner : public Object, public std::enable_shared_from_this<TaskRunner> {
 public:
  ~TaskRunner() override;

  /**
   * @brief 构造函数
   * @param name 名字
   * @param clock 引擎时钟
   */
  TaskRunner(const char *name, const std::shared_ptr<EngineClock> &clock);

  /**
   * @brief 获取当前线程正在执行任务的TaskRunner实例
   */
  static std::shared_ptr<TaskRunner> GetCurrent();

  /**
   * @brief 获取名字
   */
  const char *GetName() const { return name_; }

  /**
   * @brief 获取引擎时钟
   */
  std::shared_ptr<EngineClock> GetClock() const { return clock_; }

  /**
   * @brief 停止
   * @note 停止之后，本TaskRunner将不再接收新任务
   */
  void Stop();

  /**
   * @brief 执行下一个到时的任务
   * @note 时间栅栏（TimeFence）会对一个已到时的任务的执行与否起到决定性作用，详见SetTimeFence()方法
   * @return 是否有任务被执行
   */
  bool RunNextTask();

  /**
   * @brief 距离下一个Task达到执行条件还需等待多久时间
   */
  TDuration GetDelayOfNextTask();

  /**
   * @brief 添加一个用于唤醒任务调度者的回调函数，当TaskRunner可被调度执行时，将主动发起回调
   * @param callback 回调函数
   * @return 与回调函数一一对应的标识，在移除回调时请使用该标识
   */
  uint64_t AddWakeUpCallback(const std::function<void()> &callback) { return wake_up_notifier_->Add(callback); }

  /**
   * @brief 移除一个唤醒回调函数
   * @param callback_id 回调函数的标识
   */
  void RemoveWakeUpCallback(uint64_t callback_id) { wake_up_notifier_->Remove(callback_id); }

  /**
   * @brief 获取任务执行时间栅栏
   * @details 对于一个已到时的任务，只有当它的执行时间点小于时间栅栏时（timestamp < time_fence），才允许被执行
   *          TaskRunner默认的时间栅栏是kInvalidTaskExecutionTimeFence，即不存在时间栅栏
   */
  void SetTimeFence(const TimePoint &time_fence);

  /**
   * @brief 获取任务执行时间栅栏
   */
  TimePoint GetTimeFence() const;

  /**
   * @brief 立即提交任务（标记该任务的时间戳为：时钟当前时刻）
   * @param task 任务
   */
  virtual void PostTask(const std::shared_ptr<Task> &task);

  /**
   * @brief 延时一段时间再提交任务（标记该任务的时间戳为：时钟当前时刻 + delay）
   * @param task 任务
   * @param delay 延时时间
   */
  virtual void PostTask(const std::shared_ptr<Task> &task, const TDuration &delay);

  /**
   * @brief 立即提交任务
   * @param func 任务的具体执行函数
   * @return FutureTask实例
   */
  template <typename R = void>
  inline std::shared_ptr<FutureTask<R>> PostTask(const TaskFunctionType<R> &func) {
    auto task = TDF_MAKE_SHARED(FutureTask<R>, func);
    PostTask(task);
    return task;
  }

  /**
   * @brief 延时一段时间再提交任务
   * @param func 任务的具体执行函数
   * @param delay 延时时间
   * @return FutureTask实例
   */
  template <typename R = void>
  inline std::shared_ptr<FutureTask<R>> PostTask(const TaskFunctionType<R> &func, const TDuration &delay) {
    auto task = TDF_MAKE_SHARED(FutureTask<R>, func);
    PostTask(task, delay);
    return task;
  }

  /**
   * @brief 若本方法在调用之时已经在指定的TaskRunner的作用域内，则立刻执行任务，否则，将任务投递到TaskRunner后立刻返回
   * @param func 任务的具体执行函数，不可为nullptr
   * @result 若任务立即执行，则返回nullptr；若任务被投递到TaskRunner，返回FutureTask实例
   */
  template <typename R = void>
  inline std::shared_ptr<FutureTask<R>> ExecuteOrPostTask(const TaskFunctionType<R> &func) {
    auto task = TDF_MAKE_SHARED(FutureTask<R>, func);
    if (TaskRunner::GetCurrent().get() == this) {
      task->Run();
      return nullptr;
    }
    PostTask(task);
    return task;
  }

  /**
   * @brief
   * 若本方法在调用之时已经在指定的TaskRunner的作用域内，则立刻执行任务，否则，将任务投递到TaskRunner后并等待任务执行完毕
   * @param func 任务的具体执行函数，不可为nullptr
   * @return func的返回值
   */
  template <typename R = void>
  inline R Execute(const TaskFunctionType<R> &func) {
    auto task = TDF_MAKE_SHARED(FutureTask<R>, func);
    if (TaskRunner::GetCurrent().get() == this) {
      task->Run();
    } else {
      PostTask(task);
    }
    return task->Wait();
  }

 protected:
  struct Item {
    std::shared_ptr<Task> task;
    uint64_t sequence_id = 0;
    TimePoint timestamp = TimePoint(TDuration(0));
  };

  struct Compare {
    bool operator()(const Item &left, const Item &right) const {
      if (left.timestamp == right.timestamp) {
        return left.sequence_id > right.sequence_id;
      }
      return left.timestamp > right.timestamp;
    }
  };

  const char *name_;
  std::shared_ptr<EngineClock> clock_;
  bool is_stopped_ = false;
  uint64_t sequence_id_ = 0;
  std::queue<Item> pending_task_queue_;
  std::priority_queue<Item, std::vector<Item>, Compare> delayed_task_queue_;
  using WakeUpNotifier = CallbackNotifier<>;
  std::shared_ptr<WakeUpNotifier> wake_up_notifier_;
  TimePoint time_fence_ = kInvalidTaskExecutionTimeFence;
  mutable std::mutex mutex_;

  void PurgeDelayedTasks();
  void RunTask(const Item &item);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
