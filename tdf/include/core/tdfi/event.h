//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/macro.h"
#include "core/common/reflection.h"
#include "core/common/time.h"
#include "core/platform/common/keyboard/keyboard.h"
#include "core/platform/common/keyboard/keyboard_constants.h"
#include "core/platform/common/viewport_metrics.h"
#include "core/support/gesture/event/pointer_data.h"

namespace tdfcore {

/**
 * @brief 事件产生的源头
 */
enum class EventSource : uint8_t {
  // 默认值，表示由真正的事件源所产生的事件，并非模拟事件
  kDefault = 0,

  // 由回放器模拟出来的事件
  kReplay = 1,
};

/**
 * @brief 事件
 */
class Event : public Object, public Reflectable {
  TDF_REFF_CLASS_META(Event)

 public:
  explicit Event(TimePoint timestamp) : timestamp_(timestamp) {}
  ~Event() override = default;

  /**
   * @brief 获取时间戳
   * @return 时间戳
   */
  constexpr TimePoint GetTimestamp() const { return timestamp_; }

  /**
   * @brief 设置事件产生的源头
   * @detail
   */
  void SetSource(EventSource source) { source_ = source; }

  /**
   * @brief 获取事件产生的源头
   */
  constexpr EventSource GetSource() const { return source_; }

 protected:
  TimePoint timestamp_;
  EventSource source_ = EventSource::kDefault;
};

/**
 * @brief 指针事件
 * 触摸、鼠标点击、触控板操作、手写笔触控等都会生成该事件
 */
class PointerEvent : public Event {
  TDF_REFF_CLASS_META(PointerEvent)
 public:
  PointerEvent(TimePoint timestamp, const std::vector<PointerData>& pointer_data_list)
      : Event(timestamp), pointer_data_list_(pointer_data_list) {}

  /**
   * @brief 获取指针数据
   * @return 指针数据
   */
  constexpr const std::vector<PointerData>& GetPointerDataList() const { return pointer_data_list_; }

  /**
   * @brief 序列化与反序列化
   */
  TDF_SERIALIZATION(pointer_data_list_);

 protected:
  std::vector<PointerData> pointer_data_list_;
};

/**
 * @brief 键盘动作事件
 */
class KeyboardActionEvent : public Event {
  TDF_REFF_CLASS_META(KeyboardActionEvent)

 public:
  KeyboardActionEvent(TimePoint timestamp, KeyboardAction action) : Event(timestamp), keyboard_action_(action) {}

  /**
   * @brief 获取键盘动作
   * @return 键盘动作
   */
  constexpr KeyboardAction GetKeyboardAction() const { return keyboard_action_; }

  /**
   * @brief 序列化与反序列化
   */
  TDF_SERIALIZATION(TDF_SERIALIZABLE_ENUM(keyboard_action_, KeyboardAction, uint8_t));

 protected:
  KeyboardAction keyboard_action_;
};

/**
 * @brief 键盘编辑状态事件
 */
class KeyboardEditingStateEvent : public Event {
  TDF_REFF_CLASS_META(KeyboardEditingStateEvent)

 public:
  KeyboardEditingStateEvent(TimePoint timestamp, int64_t client_id, const EditingState& editing_state)
      : Event(timestamp), client_id_(client_id), editing_state_(editing_state) {}

  /**
   * @brief 获取键盘编辑状态
   * @return 编辑状态
   */
  constexpr const EditingState& GetEditingState() const { return editing_state_; }

  /**
   * @brief 获取Client ID
   * @return Client ID
   */
  constexpr int64_t GetClientId() const { return client_id_; }

  /**
   * @brief 序列化与反序列化
   */
  TDF_SERIALIZATION(client_id_, editing_state_);

 protected:
  int64_t client_id_;
  EditingState editing_state_;
};

/**
 * @brief 键盘状态事件
 */
class KeyboardStateEvent : public Event {
  TDF_REFF_CLASS_META(KeyboardStateEvent)
 public:
  enum class State {
    /// 正在展示
    kShowing,
    /// 没在展示
    kHiding
  };

  KeyboardStateEvent(TimePoint timestamp, State state) : Event(timestamp), state_(state) {}

  /**
   * @brief 获取键盘编辑状态
   * @return 编辑状态
   */
  constexpr const State GetState() const { return state_; }

 protected:
  State state_;
};

/***
 * @brief 键盘按键事件
 */
class KeyboardEvent : public Event {
  TDF_REFF_CLASS_META(KeyboardEvent)

  // 按键类型
  enum class EventType {
    kDown,  // 按键按下
    kUp     // 按键释放
  };

  /***
   * @brief 构造函数
   * @param timestamp 时间戳
   * @param key 按键键值
   * @param modifier_flags 修饰键
   * @param type 按键类型
   * @param characters 按键字符
   */
  KeyboardEvent(TimePoint timestamp, KeyboardKey key, ModifierFlags modifier_flags, EventType type,
                const std::string& characters)
      : Event(timestamp), key_(key), modifier_flags_(modifier_flags), type_(type), characters_(characters) {}

  /**
   * @brief 获取键盘按键
   */
  constexpr KeyboardKey GetKey() const { return key_; }

  /***
   * @brief 获取当前事件修饰按键，可以是单个或者多个ModifierFlag的组合按键
   * 对某个修饰按键ModifierFlag是否按下判断，可以通过位运算&，比如 modifier_flags_ & ModifierFlags::kShift
   */
  constexpr ModifierFlags GetModifierFlags() const { return modifier_flags_; }

  /***
   * @brief 返回按键类型
   */
  constexpr EventType GetEventType() const { return type_; }

  /***
   * @brief 返回字符，比如"A"
   * @return
   */
  constexpr const std::string& GetCharacters() const { return characters_; }

 private:
  KeyboardKey key_;
  ModifierFlags modifier_flags_;
  EventType type_;
  std::string characters_;
};

/**
 * @brief 运行循环执行前事件
 */
class PreRunLoopEvent : public Event {
  TDF_REFF_CLASS_META(PreRunLoopEvent)
 public:
  explicit PreRunLoopEvent(TimePoint timestamp) : Event(timestamp) {}
};

/**
 * @brief 运行循环执行后事件
 */
class PostRunLoopEvent : public Event {
  TDF_REFF_CLASS_META(PostRunLoopEvent)
 public:
  explicit PostRunLoopEvent(TimePoint timestamp) : Event(timestamp) {}
};

/**
 * @brief Build前事件
 * 在UI线程发送
 * Build阶段开始前会生成该事件
 */
class TDF_API PreBuildEvent : public Event {
  TDF_REFF_CLASS_META(PreBuildEvent)
 public:
  explicit PreBuildEvent(TimePoint timestamp) : Event(timestamp) {}
};

/**
 * @brief Build后事件
 * 在UI线程发送
 * Build阶段结束后会生成该事件
 */
class TDF_API PostBuildEvent : public Event {
  TDF_REFF_CLASS_META(PostBuildEvent)
 public:
  explicit PostBuildEvent(TimePoint timestamp) : Event(timestamp) {}
};

/**
 * @brief Paint前事件
 * 在UI线程发送
 * Paint阶段开始前会生成该事件
 */
class TDF_API PrePaintEvent : public Event {
  TDF_REFF_CLASS_META(PrePaintEvent)
 public:
  explicit PrePaintEvent(TimePoint timestamp) : Event(timestamp) {}
};

/**
 * @brief Paint后事件
 * 在UI线程发送
 * Paint阶段结束后会生成该事件
 */
class TDF_API PostPaintEvent : public Event {
  TDF_REFF_CLASS_META(PostPaintEvent)
 public:
  explicit PostPaintEvent(TimePoint timestamp) : Event(timestamp) {}
};

/**
 * @brief 光栅化前事件
 * 在GPU线程发送
 * 光栅化执行前（即Pipeline Raster后）会生成该事件
 */
class TDF_API PreRasterEvent : public Event {
  TDF_REFF_CLASS_META(PreRasterEvent)
 public:
  explicit PreRasterEvent(TimePoint timestamp) : Event(timestamp) {}
};

/**
 * @brief 光栅化后事件
 * 在GPU线程发送
 * 光栅化执行后（即Pipeline Raster后）会生成该事件
 */
class PostRasterEvent : public Event {
  TDF_REFF_CLASS_META(PostRasterEvent)
 public:
  explicit PostRasterEvent(TimePoint timestamp) : Event(timestamp) {}
};

/**
 * @brief 生命周期事件
 */
class LifeCycleEvent : public Event {
  TDF_REFF_CLASS_META(LifeCycleEvent)
 public:
  enum class State {
    /// 应用程序进入后台
    kPause,
    /// 应用程序进入前台
    kResume,
    /// 应用程序不可交互
    kInactive,
    /// 应用程序即将被销毁
    kDestroy
  };

  LifeCycleEvent(TimePoint timestamp, State state) : Event(timestamp), state_(state) {}

  /**
   * @brief 获取应用程序状态
   * @return 应用程序状态
   */
  constexpr State GetState() const { return state_; }

 protected:
  State state_;
};

/**
 * @brief 视图参数事件
 */
class ViewportEvent : public Event {
  TDF_REFF_CLASS_META(ViewportEvent)
 public:
  ViewportEvent(TimePoint timestamp, ViewportMetrics viewport_metrics)
      : Event(timestamp), viewport_metrics_(viewport_metrics) {}

  /**
   * @brief 获取视图参数
   * @return 视图参数
   */
  constexpr const ViewportMetrics& GetViewportMetrics() const { return viewport_metrics_; }

  ViewportMetrics viewport_metrics_;
};

/**
 * @brief 视图方向事件
 */
class OrientationEvent : public Event {
  TDF_REFF_CLASS_META(OrientationEvent)
 public:
  enum class Orientation { kPortait, kLandscape };

  OrientationEvent(TimePoint timestamp, Orientation orientation) : Event(timestamp), orientation_(orientation) {}
  Orientation GetOrientation() { return orientation_; }

 private:
  Orientation orientation_;
};

/**
 * @brief 设备当前处于低内存事件
 */
class LowMemoryEvent : public Event {
  TDF_REFF_CLASS_META(LowMemoryEvent)
 public:
  explicit LowMemoryEvent(TimePoint timestamp) : Event(timestamp) {}
};

/**
 * @brief 拖拽文件事件
 */
class FileDragEvent : public Event {
  TDF_REFF_CLASS_META(FileDragEvent)
 public:
  // 拖拽状态
  enum class DragState {
    kDragEnter,   // 进入拖拽
    kDragUpdate,  // 拖拽过程
    kDragExit,    // 推出拖拽
    kDragDone     // 完成拖拽
  };
  // 拖拽信息
  struct DragDetails {
    DragState state;
    // 全局坐标系的坐标
    TPoint position;
    // 建议在DragState为kDragDone时取这个值
    std::vector<std::string> file_list;
    std::any extra_info;
  };
  explicit FileDragEvent(TimePoint timestamp, const DragDetails& drag_details)
      : Event(timestamp), drag_details_(drag_details) {}
  DragDetails GetDragDetails() { return drag_details_; }

 private:
  DragDetails drag_details_;
};

}  // namespace tdfcore

TDF_REFL_TYPE(tdfcore::Event, bases<>)
TDF_REFL_TYPE(tdfcore::PointerEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::KeyboardActionEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::KeyboardEditingStateEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::KeyboardStateEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::KeyboardEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::PreRunLoopEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::PostRunLoopEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::PreBuildEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::PostBuildEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::PrePaintEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::PostPaintEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::PreRasterEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::PostRasterEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::LifeCycleEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::ViewportEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::OrientationEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::LowMemoryEvent, bases<tdfcore::Event>)
TDF_REFL_TYPE(tdfcore::FileDragEvent, bases<tdfcore::Event>)
