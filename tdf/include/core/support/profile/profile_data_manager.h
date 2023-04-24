//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <list>
#include <map>
#include <stack>
#include <vector>
#include "core/common/object.h"
#include "core/engine/schedule/task_runner.h"
#include "core/engine/graphics/texture.h"
#include "core/engine/schedule/thread_task_driver.h"
#include "core/support/profile/block_vector.h"
#include "core/support/profile/hash_map.h"
#include "core/support/profile/memory_object.h"
#include "core/support/profile/thread_info.h"
#include "core/support/profile/timeline_event.h"
#include "core/support/profile/task_timing_event.h"

namespace tdfcore {

/**
 * @brief 单个线程内的Timeline数据
 */
struct ThreadTimelineData {
  // 独立事件
  std::shared_ptr<BlockVector<TimelineEvent>> events;
  // 函数作用域
  std::shared_ptr<BlockVector<TimelineZone>> zones;
  // 通过将Event数据采用栈式配对法，可得到Zone数据（起始Event + 结束Event => Zone），此为配对用的栈
  std::stack<TimelineEvent> unhandled_events;

  ThreadTimelineData();
  bool IsEmpty() const;
};

/**
 * @brief 单个线程中内存原始上报事件的集合
 */
struct ThreadMemoryData {
  std::shared_ptr<BlockVector<MemoryEvent>> events;

  ThreadMemoryData();
  bool IsEmpty() const;
};

/**
 * @brief 全部内存数据
 */
struct GlobalMemoryData {
  // 当前驻留在内存中的堆对象
  HashMap<int64_t, MemoryObject, PointerHash> objects;
  // 与unhandled_memory_items一起，配合施展内存数据的合并计算操作
  std::vector<MemoryEvent> handling_events;
  // 与handling_memory_items一起，配合施展内存数据的合并计算操作
  std::vector<MemoryEvent> unhandled_events;
};

struct RunnerTaskTimingData {
  std::shared_ptr<BlockVector<TaskTimingZone>> zones;

  RunnerTaskTimingData();
  bool IsEmpty() const;
};

/**
 * @brief 性能数据的汇总计算与管理
 */
class ProfileDataManager : public std::enable_shared_from_this<ProfileDataManager> {
 public:
  ProfileDataManager() = default;
  ~ProfileDataManager();

  static std::shared_ptr<ProfileDataManager> GetInstance();

  /**
   * @brief 开启数据采集
   */
  void Start(const std::shared_ptr<TaskRunner> task_runner, const std::shared_ptr<TaskDriver> task_driver);
  /**
   * @brief 终止数据采集，释放资源
   */
  void Terminate();

  /**
   * @brief 触发一次收集数据的操作
   */
  void Collect();

  /**
   * @brief 是否已开启
   */
  bool IsValid() { return is_valid_; }

  /**
   * @brief 获取指定时间之后产生的TimelineEvent数据
   */
  std::shared_ptr<std::vector<TimelineEvent>> GetTimelineEvents(const RealTimePoint &after_time) const;

  /**
   * @brief 获取指定时间之后产生的TimelineZone数据（以TimelineZone的end_timestamp为准）
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
   * @brief 获取各个TaskRunner中的Task执行耗时与延迟数据
   */
  TaskTimingZoneMap GetTaskTimingZones(const RealTimePoint &after_time) const;

  /**
   * @brief 为Texture创建截图，并上传Tracy
   */
  static void TextureSnapshotAndTracy(const std::shared_ptr<Device> &device, std::shared_ptr<Texture> texture);

 private:
  void ScheduleCollect();
  // 函数耗时数据
  void CollectTimelineData();
  void MatchTimelineEvent(TimelineEvent *event, uint32_t tid, ThreadTimelineData &data);
  void RecycleCachedTimelineData();
  // 内存数据
  void CollectMemoryData();
  bool MatchMemoryEvent(MemoryEvent *event);
  void RecycleCachedMemoryData();
  // Task耗时数据
  void CollectTaskTimingData();
  void RecycleTaskTimingData();

  // 对于内存和Timeline类的性能数据，ProfileDataManager会为之缓存最近2秒的数据，2秒之前的数据会被丢弃
  constexpr static const TDuration kDataCacheDuration = std::chrono::seconds(2);
  // ProfileDataManager单例
  static std::shared_ptr<ProfileDataManager> instance;
  // Timeline数据缓存
  std::map<ThreadId, ThreadTimelineData> thread_timeline_data_map_;
  // 内存原始事件的缓存
  std::map<ThreadId, ThreadMemoryData> thread_memory_data_map_;
  // 内存中所有对象集合
  GlobalMemoryData global_memory_data_;
  // Task耗时所有对象集合
  std::map<const char*, RunnerTaskTimingData> runner_task_timing_map_;
  bool is_valid_ = false;
  std::shared_ptr<TaskRunner> task_runner_;
  std::shared_ptr<TaskDriver> task_driver_;
  mutable std::mutex data_mutex_;

  template <typename T>
  inline std::shared_ptr<std::vector<T>> MakeVector(const std::vector<std::pair<const T *, size_t>> &slices,
                                                    size_t item_count) const {
    auto result = std::make_shared<std::vector<T>>(item_count);
    T *data = result->data();
    for (const auto &slice : slices) {
      memcpy(reinterpret_cast<void *>(data), reinterpret_cast<const void *>(slice.first), sizeof(T) * slice.second);
      data += slice.second;
    }
    return result;
  }

  template <typename T>
  inline std::shared_ptr<std::vector<T>> GetDataFromBlockVectors(
      const std::vector<std::shared_ptr<BlockVector<T>>> &block_vectors,
      const std::function<bool(const T *item)> &matcher) const {
    std::lock_guard<std::mutex> lock(data_mutex_);
    size_t count = 0;
    std::vector<std::pair<const T *, size_t>> slices;
    // 从每一个BlockVector中搜索出想要获取的（BlockVector::Block类型）数据片段
    for (const auto &block_vector : block_vectors) {
      for (const auto &block : block_vector->GetBlocks()) {
        auto index = block.FindFirstIndex(matcher);
        if (index < block.size) {
          count += block.size - index;
          slices.emplace_back(std::pair<T *, size_t>(block.items + index, block.size - index));
        }
      }
    }
    // 将这些数据片段拷贝合并入一个std::vector中
    return MakeVector(slices, count);
  }

  template <typename T>
  inline void RecycleCachedThreadData(std::map<ThreadId, T> &thread_data_map,
                                      const std::function<void(const T &thread_data)> &recycler) {
    for (auto iter = thread_data_map.begin(); iter != thread_data_map.end();) {
      recycler(iter->second);
      if (iter->second.IsEmpty()) {
        iter = thread_data_map.erase(iter);
      } else {
        iter++;
      }
    }
  }

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
