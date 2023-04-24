//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <functional>
#include <list>
#include <mutex>
#include <unordered_map>
#include "core/common/object.h"
#include "core/tdfi/event.h"

namespace tdfcore {
/**
 * @brief 事件派发行为
 */
enum class EventDispatchBehavior {
  /// 继续派发
  kContinue,
  /// 停止派发
  kStop
};

/**
 * @brief 事件中心
 */
class EventCenter : public Object {
 public:
  /**
   * @brief 监听者
   * 参数说明：(事件、监听者ID)
   * 返回值说明：事件派发行为
   * 由该监听者决定是否继续派发下去，`kContinue`代表继续派发，`kStop`代表停止派发
   */
  using EventListener = std::function<EventDispatchBehavior(std::shared_ptr<Event>, uint64_t)>;

  EventCenter() = default;
  ~EventCenter() override = default;

  /**
   * @brief 发送事件
   * @param event 事件
   */
  void SendEvent(const std::shared_ptr<Event> &event);

  /**
   * @brief 添加监听者
   * 注意：后添加先被执行
   * @param type 事件类型
   * @param listener 监听者
   * @return 监听者ID
   */
  uint64_t AddListener(Type type, const EventListener &listener);

  /**
   * @brief 移除监听者
   * @param type 事件类型
   * @param id 监听者ID
   */
  void RemoveListener(Type type, uint64_t id);

 private:
  struct ListenerData {
    uint64_t id;
    EventListener listener;
  };

  std::unordered_map<std::string, std::list<ListenerData>> listeners_map_;
  std::unordered_map<uint64_t, typename std::list<ListenerData>::iterator> iterators_map_;
  std::recursive_mutex mutex_;
};
}  // namespace tdfcore
