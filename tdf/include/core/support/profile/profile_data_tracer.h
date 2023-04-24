//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <functional>
#include "core/common/macro.h"
#include "core/support/profile/concurrent_queue.h"
#include "core/support/profile/task_timing_event.h"
#include "core/support/profile/timeline_event.h"
#include "core/support/profile/memory_object.h"

namespace tdfcore {

class ProfileDataTracer {
 public:
  using TimelineQueueItemReader = std::function<void(TimelineEvent *item, size_t count, uint32_t tid)>;

  TDF_FORCE_INLINE void DequeueTimelineQueue(const TimelineQueueItemReader &reader) {
    Dequeue(timeline_queue_, timeline_queue_consumer_, reader);
  }

  using MemoryQueueItemReader = std::function<void(MemoryEvent *item, size_t count, uint32_t tid)>;

  TDF_FORCE_INLINE void DequeueMemoryQueue(const MemoryQueueItemReader &reader) {
    Dequeue(memory_queue_, memory_queue_consumer_, reader);
  }

  using TaskTimingQueueItemReader = std::function<void(TaskTimingZone *item, size_t count, uint32_t tid)>;

  TDF_FORCE_INLINE void DequeueTaskTimingQueue(const TaskTimingQueueItemReader &reader) {
    Dequeue(task_timing_queue_, task_timing_queue_consumer_, reader);
  }

 private:
  moodycamel::ConcurrentQueue<TimelineEvent> timeline_queue_;
  moodycamel::ConcurrentQueue<MemoryEvent> memory_queue_;
  moodycamel::ConcurrentQueue<TaskTimingZone> task_timing_queue_;
  moodycamel::ConsumerToken timeline_queue_consumer_ = moodycamel::ConsumerToken(timeline_queue_);
  moodycamel::ConsumerToken memory_queue_consumer_ = moodycamel::ConsumerToken(memory_queue_);
  moodycamel::ConsumerToken task_timing_queue_consumer_ = moodycamel::ConsumerToken(task_timing_queue_);

  template <typename T>
  void Dequeue(moodycamel::ConcurrentQueue<T> &queue, moodycamel::ConsumerToken &token,
               const std::function<void(T *item, size_t count, uint32_t tid)> &reader) {
    size_t count = 0;
    do {
      uint32_t tid = 0;
      count = queue.try_dequeue_bulk_single(
          token, [&tid](const uint32_t &thread_id) { tid = thread_id; },
          [reader, &tid](T *item, size_t sz) { reader(item, sz, tid); });
    } while (count > 0);
  }

  friend struct ThreadProfileData;
};

ProfileDataTracer &GetProfileDataTracer();

moodycamel::ConcurrentQueue<TimelineEvent>::ExplicitProducer *GetTimelineQueueProducer();

moodycamel::ConcurrentQueue<MemoryEvent>::ExplicitProducer *GetMemoryQueueProducer();

moodycamel::ConcurrentQueue<TaskTimingZone>::ExplicitProducer *GetTaskTimingQueueProducer();

}  // namespace tdfcore