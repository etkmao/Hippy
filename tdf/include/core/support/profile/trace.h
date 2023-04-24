//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/support/profile/profile_data_tracer.h"
#include "core/support/profile/profile_data_manager.h"

#define TDF_TRACE_MAKE_QUEUE_ITEM(token)                 \
  ::tdfcore::moodycamel::ConcurrentQueueDefaultTraits::index_t magic; \
  auto &tail = token->get_tail_index();                               \
  auto item = token->enqueue_begin(magic);

#define TDF_TRACE_COMMIT_QUEUE_ITEM tail.store(magic + 1, std::memory_order_release)

#define TDF_TRACE_GET_CLOCK_NOW std::chrono::steady_clock::now().time_since_epoch().count()

namespace tdfcore {

template <typename T>
TDF_FORCE_INLINE void QueueItemSetField(void *field_ptr, T val) {
  memcpy(field_ptr, &val, sizeof(T));
}

class TimelineZoneTracer {
 public:
  TimelineZoneTracer(const TimelineZoneTracer &) = delete;
  TimelineZoneTracer(TimelineZoneTracer &&) = delete;
  TimelineZoneTracer &operator=(const TimelineZoneTracer &) = delete;
  TimelineZoneTracer &operator=(TimelineZoneTracer &&) = delete;

  TDF_FORCE_INLINE TimelineZoneTracer(const char *label, int64_t group_id = 0) {
    auto producer = GetTimelineQueueProducer();
    TDF_TRACE_MAKE_QUEUE_ITEM(producer)
    QueueItemSetField(&item->type, TimelineEventType::kBegin);
    QueueItemSetField(&item->timestamp, TDF_TRACE_GET_CLOCK_NOW);
    QueueItemSetField(&item->label, label);
    QueueItemSetField(&item->group_id, group_id);
    TDF_TRACE_COMMIT_QUEUE_ITEM;
  }

  TDF_FORCE_INLINE ~TimelineZoneTracer() {
    auto producer = GetTimelineQueueProducer();
    TDF_TRACE_MAKE_QUEUE_ITEM(producer)
    QueueItemSetField(&item->type, TimelineEventType::kEnd);
    QueueItemSetField(&item->timestamp, TDF_TRACE_GET_CLOCK_NOW);
    TDF_TRACE_COMMIT_QUEUE_ITEM;
  }
};

TDF_FORCE_INLINE void TraceMemoryAlloc(const void *ptr, int64_t bytes, const char *type, const char *category,
                                       const char *file, const int64_t line) {
  auto producer = GetMemoryQueueProducer();
  TDF_TRACE_MAKE_QUEUE_ITEM(producer)
  QueueItemSetField(&item->type, MemoryEventType::kAlloc);
  QueueItemSetField(&item->object.pointer, ptr);
  QueueItemSetField(&item->timestamp, TDF_TRACE_GET_CLOCK_NOW);
  QueueItemSetField(&item->object.bytes, bytes);
  QueueItemSetField(&item->object.type, type);
  QueueItemSetField(&item->object.category, category);
  QueueItemSetField(&item->object.alloc_location.file, file);
  QueueItemSetField(&item->object.alloc_location.line, line);
  TDF_TRACE_COMMIT_QUEUE_ITEM;
}

TDF_FORCE_INLINE void TraceMemoryFree(const void *ptr) {
  auto producer = GetMemoryQueueProducer();
  TDF_TRACE_MAKE_QUEUE_ITEM(producer)
  QueueItemSetField(&item->type, MemoryEventType::kFree);
  QueueItemSetField(&item->object.pointer, ptr);
  QueueItemSetField(&item->timestamp, TDF_TRACE_GET_CLOCK_NOW);
  TDF_TRACE_COMMIT_QUEUE_ITEM;
}

TDF_FORCE_INLINE void TextureSnapshotAndTracy(const std::shared_ptr<Device> &device, std::shared_ptr<Texture> texture) {
  ProfileDataManager::TextureSnapshotAndTracy(device, texture);
}

class TaskTimingZoneTracer {
 public:
  TDF_DISALLOW_COPY_ASSIGN_AND_MOVE(TaskTimingZoneTracer);
  TDF_FORCE_INLINE TaskTimingZoneTracer(const char* &task_runner,
                                        const TimePoint &task_exec_time,
                                        const std::shared_ptr<EngineClock> &clock)
      : task_runner_(task_runner), task_exec_time_(task_exec_time), clock_(clock) {
    task_actual_time_ = clock_->Now();
    timestamp_ = RealTimeClock::now();
  }

  TDF_FORCE_INLINE ~TaskTimingZoneTracer() {
    auto delay = task_actual_time_ - task_exec_time_;
    auto cost = clock_->Now() - task_actual_time_;
    auto producer = GetTaskTimingQueueProducer();
    TDF_TRACE_MAKE_QUEUE_ITEM(producer)
    item->task_runner = task_runner_;
    QueueItemSetField(&item->execution_delay, delay);
    QueueItemSetField(&item->execution_cost, cost);
    QueueItemSetField(&item->timestamp, timestamp_);
    TDF_TRACE_COMMIT_QUEUE_ITEM;
  }

 private:
  const char* task_runner_;
  TimePoint task_actual_time_;  // 实际被执行的时间
  TimePoint task_exec_time_;   // 预期被执行的时间
  RealTimePoint timestamp_;
  std::shared_ptr<EngineClock> clock_;
};

}  // namespace tdfcore
