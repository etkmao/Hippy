//
// Copyright © 2020 com.tencent. All rights reserved.
//

#pragma once

#include <unordered_map>
#include "core/common/callback_notifier.h"
#include "core/common/reflection.h"
#include "core/engine/schedule/task_driver.h"
#include "core/engine/schedule/task_driver_run_loop.h"
#include "core/engine/schedule/task_runner.h"
#include "core/platform/common/platform.h"

namespace tdfcore {

/**
 * @brief 内核任务调度器的基类，请使用其子类
 */
class Scheduler : public Object, public Reflectable, public std::enable_shared_from_this<Scheduler> {
  TDF_REFF_CLASS_META(Scheduler)

 public:
  ~Scheduler() override = default;
  Scheduler(const Scheduler &other) = delete;

  /**
   * @brief 获取EngineClock
   */
  constexpr const std::shared_ptr<EngineClock> &GetClock() const { return clock_; }

  /**
   * @brief 获取UI TaskRunner
   * @note 所有的UI任务（排版、绘制）都应该在此TaskRunner执行
   */
  constexpr const std::shared_ptr<TaskRunner> &GetUITaskRunner() const { return ui_runner_; }

  /**
   * @brief 获取光栅化 TaskRunner
   * @note 若"config.is_raster_in_gpu_thread"为false，则会返回nullptr
   */
  constexpr const std::shared_ptr<TaskRunner> &GetRasterTaskRunner() const { return raster_runner_; }

  /**
   * @brief 获取IO TaskRunner
   * @note 繁重的任务应该在这个TaskRunner里执行
   */
  constexpr const std::shared_ptr<TaskRunner> &GetIOTaskRunner() const { return io_runner_; }

  /**
   * @brief 获取Platform TaskRunner
   * @note 平台侧任务应该在这个TaskRunner里执行
   */
  constexpr const std::shared_ptr<TaskRunner> &GetPlatformTaskRunner() const { return platform_runner_; }

  /**
   * @brief 获取执行Pipeline任务的TaskDriver
   */
  constexpr const std::shared_ptr<TaskDriver> &GetPipelineDriver() const { return pipeline_driver_; }

  /**
   * @brief 获取UI TaskDriver
   */
  constexpr const std::shared_ptr<TaskDriver> &GetUITaskDriver() const { return ui_driver_; }

  /**
   * @brief 获取光栅化 TaskDriver
   * @note 若"config.is_raster_in_gpu_thread"为false，则会返回nullptr
   */
  constexpr const std::shared_ptr<TaskDriver> &GetRasterTaskDriver() const { return raster_driver_; }

  /**
   * @brief 获取IO TaskDriver
   */
  constexpr const std::shared_ptr<TaskDriver> &GetIOTaskDriver() const { return io_driver_; }

  /**
   * @brief 获取RunLoop TaskDriver
   */
  constexpr const std::shared_ptr<TaskDriver> &GetPlatformTaskDriver() const { return platform_driver_; }

  /**
   * @brief 创建一个TaskRunner，并采用Schedule的clock作为自己的clock
   * @param name 该TaskRunner的名字
   */
  std::shared_ptr<TaskRunner> CreateTaskRunner(const char *name);

  /**
   * @brief 创建一个独占线程的TaskDriver，其调度逻辑由Schedule统一提供和管理，而生命周期由调用者自行管理
   * @param task_runners 在该TaskDriver上调度执行的TaskRunner们；
   *                     注意，仅接受那些与Schedule公用同一个时钟（Clock）对象的TaskRunner
   */
  std::shared_ptr<TaskDriver> CreateThreadTaskDriver(const std::vector<std::shared_ptr<TaskRunner>> &runners);

  /**
   * @brief 调度Pipeline任务
   * @param pipeline_func Pipeline任务执行体，该任务将被放入到一个专属的TaskDriver中调度执行
   */
  void ScheduleRenderPipeline(const std::function<void()> &pipeline_func);

  /**
   * @brief 启动调度
   */
  void Start();

  /**
   * @brief 暂停调度执行
   * @note 本函数采用阻塞方式去等待所有TaskDriver暂停执行；后续调用Resume方法可恢复至启动状态
   */
  void Pause();

  /**
   * @brief 恢复调度执行
   */
  void Resume();

  /**
   * @brief 结束调度并销毁资源，此后不可再调用本类的其它方法
   */
  void Terminate();

  /**
   * @brief 是否有效、可用
   */
  bool IsValid() const { return is_valid_; }

  /**
   * @brief 进入帧级调度运行模式
   * @detail 帧级调度运行模式：完全由外部调用者控制，以帧为单位，主动推进Scheduler的调度运行
   *   （1）调用BeginSchedulingFrames方法进入该模式，在进入的瞬间，Scheduler会自动进入暂停状态
   *   （2）外部调用者根据自己的节奏，每调用一次ScheduleFrame方法，则推动Scheduler调度运行一帧，该帧完毕后，Scheduler会再次进入暂停状态
   *   （3）调用EndSchedulingFrames方法退出该模式，恢复至正常运行模式
   */
  void BeginSchedulingFrames();

  /**
   * @brief 退出帧级调度运行模式
   */
  void EndSchedulingFrames();

  /**
   * @brief 调度运行一帧
   * @param frame_interval 本帧的时间间隔；必须大于0
   */
  void ScheduleFrame(const TDuration &frame_interval);

  /**
   * @brief TaskDriver的调度事件
   */
  enum class TaskDriverEvent {
    // 进入RunOnce的处理
    kEnterRunLoop,

    // 走出RunOnce的处理
    kLeaveRunLoop,
  };

  /**
   * @brief TaskDriver的调度事件的回调函数
   * @param driver 发生调度事件的TaskDriver
   * @param event 调度事件
   */
  using TaskDriverEventCallback = std::function<void(const std::shared_ptr<TaskDriver> &driver, TaskDriverEvent event)>;

  /**
   * @brief 添加TaskDriver调度事件的回调函数
   * @param callback 回调函数
   * @return 与该回调函数相关联的标识，可用于移除
   */
  uint64_t AddTaskDriverEventCallback(const TaskDriverEventCallback &callback) {
    return task_driver_event_notifier_->Add(callback);
  }

  /**
   * @brief 移除TaskDriver调度事件的回调函数
   * @param callback_id 与回调函数相关联的标识
   */
  void RemoveTaskDriverEventCallback(uint64_t callback_id) { task_driver_event_notifier_->Remove(callback_id); }

 protected:
  struct ScheduledTaskDriver {
    std::weak_ptr<TaskDriver> driver;
    std::shared_ptr<TaskDriverRunLoop> run_loop;
  };

  bool is_valid_ = true;
  bool is_started_ = false;
  std::mutex mutex_;
  std::shared_ptr<EngineClock> clock_;
  std::shared_ptr<TaskDriver> pipeline_driver_;
  std::shared_ptr<TaskDriver> ui_driver_;
  std::shared_ptr<TaskDriver> raster_driver_;
  std::shared_ptr<TaskDriver> io_driver_;
  std::shared_ptr<TaskDriver> platform_driver_;
  std::shared_ptr<TaskRunner> ui_runner_;
  std::shared_ptr<TaskRunner> raster_runner_;
  std::shared_ptr<TaskRunner> io_runner_;
  std::shared_ptr<TaskRunner> platform_runner_;
  std::unordered_map<const TaskDriver *, ScheduledTaskDriver> scheduled_task_drivers_;
  using TaskDriverEventNotifier = CallbackNotifier<const std::shared_ptr<TaskDriver> &, TaskDriverEvent>;
  std::shared_ptr<TaskDriverEventNotifier> task_driver_event_notifier_;
  bool is_scheduling_frames_ = false;
  std::shared_ptr<TimeCounter> original_time_counter_;
  std::shared_ptr<ManualTimeCounter> schedule_frame_time_counter_;

  explicit Scheduler(const std::shared_ptr<EngineClock> &clock);
  void InitInternalTaskRunners();
  void SetupTaskDriver(const std::shared_ptr<TaskDriver> &driver, const std::string &name);
  void ScheduleTaskRunners(const std::vector<std::shared_ptr<TaskRunner>> &task_runners,
                           const std::shared_ptr<TaskDriver> &task_driver);
  using TaskDriverVisitor = std::function<void(const std::shared_ptr<TaskDriver> &)>;
  void VisitInternalTaskDrivers(const TaskDriverVisitor &visitor);
  using TaskDriverRunLoopVisitor =
      std::function<void(const std::shared_ptr<TaskDriver> &, const std::shared_ptr<TaskDriverRunLoop> &)>;
  void VisitTaskDriverRunLoops(const TaskDriverRunLoopVisitor &visitor);
};

/**
 * @brief 内核任务调度器
 * @param 任务调度模型：UI-TaskRunner可独占一个TaskDriver，Raster-TaskRunner与Platform-TaskRunner共用同一个TaskDriver
 */
class DefaultScheduler : public Scheduler {
  TDF_REFF_CLASS_META(DefaultScheduler)

 public:
  ~DefaultScheduler() override = default;
  DefaultScheduler(const DefaultScheduler &other) = delete;

  /**
   * @brief 可被配置的TaskDriver
   * @param Scheduler优先采用配置指定的TaskDriver，若配置为nullptr，则Scheduler会默认创建一个
   */
  struct TaskDrivers {
    std::shared_ptr<TaskDriver> render_pipeline;
    std::shared_ptr<TaskDriver> platform;
    std::shared_ptr<TaskDriver> ui;
    std::shared_ptr<TaskDriver> io;
  };

 protected:
  /**
   * @brief 构造函数
   * @param platform 平台对象
   * @param drivers TaskDriver的配置
   * @param clock 引擎时钟；若为nullptr，则默认创建一个
   */
  explicit DefaultScheduler(const std::shared_ptr<Platform> &platform, const TaskDrivers &drivers = TaskDrivers(),
                            const std::shared_ptr<EngineClock> &clock = nullptr);

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 内核任务调度器
 * @param 任务调度模型：UI-TaskRunner、Raster-TaskRunner、Platform-TaskRunner均可分别独占一个TaskDriver
 */
class PerformancePriorityScheduler : public Scheduler {
  TDF_REFF_CLASS_META(PerformancePriorityScheduler)

 public:
  ~PerformancePriorityScheduler() override = default;
  PerformancePriorityScheduler(const PerformancePriorityScheduler &other) = delete;

  /**
   * @brief 可被配置的TaskDriver
   * @param Scheduler优先采用配置指定的TaskDriver，若配置为nullptr，则Scheduler会默认创建一个
   */
  struct TaskDrivers {
    std::shared_ptr<TaskDriver> render_pipeline;
    std::shared_ptr<TaskDriver> platform;
    std::shared_ptr<TaskDriver> ui;
    std::shared_ptr<TaskDriver> raster;
    std::shared_ptr<TaskDriver> io;
  };

 protected:
  /**
   * @brief 构造函数
   * @param platform 平台对象
   * @param drivers TaskDriver的配置
   * @param clock 引擎时钟；若为nullptr，则默认创建一个
   */
  explicit PerformancePriorityScheduler(const std::shared_ptr<Platform> &platform,
                                        const TaskDrivers &drivers = TaskDrivers(),
                                        const std::shared_ptr<EngineClock> &clock = nullptr);

  FRIEND_OF_TDF_ALLOC;
};

/**
 * @brief 内核任务调度器
 * @param 任务调度模型：Pipeline、UI-TaskRunner、Raster-TaskRunner、Platform-TaskRunner共用同一个TaskDriver
 */
class SynchronicityPriorityScheduler : public Scheduler {
  TDF_REFF_CLASS_META(SynchronicityPriorityScheduler)

 public:
  ~SynchronicityPriorityScheduler() override = default;
  SynchronicityPriorityScheduler(const SynchronicityPriorityScheduler &other) = delete;

  /**
   * @brief 可被配置的TaskDriver
   * @param Scheduler优先采用配置指定的TaskDriver，若配置为nullptr，则Scheduler会默认创建一个
   */
  struct TaskDrivers {
    std::shared_ptr<TaskDriver> render_pipeline;
    std::shared_ptr<TaskDriver> io;
  };

 protected:
  /**
   * @brief 构造函数
   * @param platform 平台对象
   * @param drivers TaskDriver的配置
   * @param clock 引擎时钟；若为nullptr，则默认创建一个
   */
  explicit SynchronicityPriorityScheduler(const std::shared_ptr<Platform> &platform,
                                          const TaskDrivers &drivers = TaskDrivers(),
                                          const std::shared_ptr<EngineClock> &clock = nullptr);

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::Scheduler, bases<>)
TDF_REFL_END(tdfcore::Scheduler)

TDF_REFL_DEFINE(tdfcore::DefaultScheduler, bases<tdfcore::Scheduler>)
TDF_REFL_END(tdfcore::DefaultScheduler)

TDF_REFL_DEFINE(tdfcore::PerformancePriorityScheduler, bases<tdfcore::Scheduler>)
TDF_REFL_END(tdfcore::PerformancePriorityScheduler)

TDF_REFL_DEFINE(tdfcore::SynchronicityPriorityScheduler, bases<tdfcore::Scheduler>)
TDF_REFL_END(tdfcore::SynchronicityPriorityScheduler)
