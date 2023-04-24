//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <unordered_map>
#include <memory>
#include <mutex>
#include <queue>
#include "core/support/gesture/arena/gesture_arena_manager.h"
#include "core/engine/schedule/render_pipeline.h"
#include "core/support/gesture/event/pointer_state.h"
#include "core/support/gesture/event/pointer_data.h"
#include "core/support/gesture/recognizer/gesture_recognizer.h"
#include "core/engine/hittest/hit_test.h"

namespace tdfcore {
class PointerHandler : public Object, public std::enable_shared_from_this<PointerHandler> {
 public:
  explicit PointerHandler(std::shared_ptr<RenderPipeline> render_pipeline);
  ~PointerHandler() override = default;

  /**
   * @brief 从shell层接收触摸事件的源数据
   *
   * @param events 触摸事件源数据
   */
  void HandlePointerDataPacket(const std::vector<PointerData> &data_vector);

  /**
  * @brief
  * 内核是否可以响应手势事件，如果相应在 Native RenderObject 是属于内核不能响应事件状态
  *
  * @param data 事件数据
  * @return 是否可以响应手势事件
  */
  bool CanConsumePointerData(const PointerData &data);

 private:
  std::shared_ptr<RenderPipeline> render_pipeline_;
  std::queue<PointerData> pending_pointer_data_queue_;
  std::unordered_map<PointerID, std::shared_ptr<HitTestResult>> hit_tests_;
  std::shared_ptr<GestureArenaManager> gesture_arena_manager_;
  std::unordered_map<int64_t, std::shared_ptr<PointerState>> pointer_states_map_;

  void FlushPointerDataQueue();
  void HandlePointerData(PointerData &data);
  void HandleHoverPointerData(PointerData &data,
                              const std::shared_ptr<HitTestResult> &last_result,
                              const std::shared_ptr<HitTestResult> &new_result);
  void DispatchRecognizers(const std::shared_ptr<HitTestResult>& result,
                           const PointerData &data);
  std::shared_ptr<HitTestResult> HitTest(TPoint position);
};

}  // namespace tdfcore
