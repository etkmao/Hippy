//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <map>
#include <vector>
#include <list>
#include "core/common/object.h"
#include "core/engine/schedule/scheduler.h"
#include "core/engine/schedule/task_runner.h"
#include "core/engine/schedule/thread_task_driver.h"
#include "core/engine/graphics/device.h"
#include "core/support/profile/memory_object.h"
#include "core/support/profile/thread_info.h"
#include "core/support/profile/timeline_event.h"
#include "core/support/profile/graphic_memory_object.h"
#include "core/support/profile/task_timing_event.h"

namespace tdfcore {

class ProfileDataManager;

class Profiler : public Object, public std::enable_shared_from_this<Profiler> {
 public:
  ~Profiler() override = default;
  Profiler(GrDirectContext *gr_context, const std::shared_ptr<Scheduler> &scheduler);

  /**
   * @brief 获取指定时间之后产生的TimelineEvent数据
   * @param after_time 指定时间
   */
  std::shared_ptr<std::vector<TimelineEvent>> GetTimelineEvents(const RealTimePoint &after_time) const;

  /**
   * @brief 获取指定时间之后产生的TimelineZone数据
   * @param after_time 指定时间，以TimelineZone的end_timestamp为准
   */
  std::shared_ptr<std::vector<TimelineZone>> GetTimelineZones(const RealTimePoint &after_time) const;

  /**
   * @brief 获取当前内存中驻留的堆对象
   */
  std::shared_ptr<MemoryObjectMap> GetMemoryObjects() const;

  /**
   * @brief 获取内存上报的原始数据，包括分配和释放两种事件
   */
  std::shared_ptr<std::vector<MemoryEvent>> GetMemoryEvents(const RealTimePoint &after_time) const;

  /**
   * @brief 获取当前Skia内存中驻留的对象
   */
  std::shared_ptr<std::list<GraphicMemoryObjects>> GetGraphicMemoryObjects() const;

  /**
   * @brief 获取一个时间区间内Task运行耗时和延迟数据
   */
  TaskTimingZoneMap GetTaskTimingZones(const RealTimePoint &after_time) const;

  /**
   * @brief Profiler的使用者，可以使用该TaskRunner执行数据采集任务
   */
  std::shared_ptr<TaskRunner> GetTaskRunner() const;

  /**
   * @brief 获取指定线程的信息
   * @param thread_id 线程Id
   */
  ThreadInfo GetThreadInfo(const ThreadId thread_id) const;

  /**
   * @brief 获取所有线程的信息
   */
  std::vector<ThreadInfo> GetAllThreadInfos() const;

  /**
   * @brief 批量更新线程信息
   * @param task_runners 指定更新TaskRunner所在的线程
   */
  void UpdateThreadInfosByTaskRunners(const std::vector<std::shared_ptr<TaskRunner>> &task_runners);

  /**
   * @brief 停止采集，使用者必须显式调用
   */
  void Terminate();

 protected:
  void Init() override;
  void CollectGraphicMemory();
  void ScheduleCollectGraphicMemory();
  void AutoStartInNeeded();

 private:
  std::shared_ptr<TaskRunner> task_runner_;
  std::shared_ptr<TaskDriver> task_driver_;
  std::shared_ptr<TaskRunner> manager_task_runner_;
  std::shared_ptr<TaskDriver> manager_task_driver_;
  mutable std::mutex thread_info_mutex_;
  mutable std::mutex graphic_memory_mutex_;
  std::map<ThreadId, ThreadInfo> thread_infos_;
  GrDirectContext *gr_context_;
  std::shared_ptr<std::list<GraphicMemoryObjects>> graphic_cache_;
  std::shared_ptr<Scheduler> scheduler_;
  bool is_valid_;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
