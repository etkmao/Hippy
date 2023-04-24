//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <vector>
#include "core/tdfi/event.h"

/**
 * @discuss
 *   将"Android侧的API常量"到"C++侧的tdfcore的常量"之间的映射逻辑放到C++侧的考虑是：
 *   （1）Android系统提供的API显然比tdfcore提供的API在稳固性上要高，几乎可以认为Android侧的这些API常量的取值永远不会变
 *   （2）Android端的平台侧处理代码本就包含Java与C++代码，在分工上，令Java代码对接Android的API，令C++代码实现tdfcore的需求
 *   （3）将Android侧API提供的数据无雕饰的直接给到C++侧，以后tdfcore无论怎样变更自己的内部逻辑，都只要改C++侧的代码即可
 *   （4）一旦tdfcore变更自己的内部逻辑，修改代码实现后，有更大几率在开发与编译期就发现问题，而不是拖到上线运行时才发现问题
 */

namespace tdfcore {

/**
 * @brief 代表Android框架下的KeyEvent相关的一系列定义
 * @note AndroidKeyEvent内的各枚举的取值，是严格参照Android框架下的KeyEvent.java文件中的定义，不可随意修改！
 *       结构体内的字段的声明顺序与类型，需与Java侧序列化数据的内存布局结构一致，见TDFPlatformEvnetHandler.java文件
 */
struct alignas(8) AndroidKeyEvent {
  /**
   * @brief 以下枚举量对应的是Android框架下的KeyEvent类内的"ACTION_"前缀的常量定义
   */
  enum class Action : int64_t {
    kDown = 0,
    kUp = 1,
  };

  /**
   * @brief 以下枚举量对应的是Android框架下的KeyEvent类内的"META_"前缀的常量定义
   */
  enum class MetaMask : int64_t {
    kShift = 0x1 | 0x40 | 0x80,
    kAlt = 0x02 | 0x10 | 0x20,
    kControl = 0x1000 | 0x2000 | 0x4000,
    kMeta = 0x10000 | 0x20000 | 0x40000,
    kCapsLock = 0x100000,
  };

  /**
   * @brief 修饰类按键状态，取值为MetaMask的组合
   */
  using MetaState = int64_t;

  /**
   * @note 以下字段的声明顺序与类型，需与Java侧序列化数据的内存布局结构一致，见TDFPlatformEvnetHandler.java文件
   */
  Action action;
  int64_t key_code;
  MetaState meta_state;
  int64_t character;

  /**
   * @brief 将数据反序列化，生成AndroidKeyEvent实例
   */
  static AndroidKeyEvent MakeFromData(const uint8_t *data, size_t size);
};

/**
 * @brief 代表Android框架下的MotionEvent相关的一系列定义
 * @note AndroidKeyEvent内的各枚举的取值，是严格参照Android框架下的KeyEvent.java文件中的定义，不可随意修改！
 *       结构体内的字段的声明顺序与类型，需与Java侧序列化数据的内存布局结构一致，见TDFPlatformEvnetHandler.java文件
 */
struct AndroidMotionEvent {
  /**
   * @brief 以下枚举量对应的是Android框架下的MotionEvent类内的"ACTION_"前缀的常量定义
   */
  enum class Action : int64_t {
    kDown = 0,
    kUp = 1,
    kMove = 2,
    kCancel = 3,
    kPointerDown = 5,
    kPointerUp = 6,
    kHoverMove = 7,
    kScroll = 8,
  };

  /**
   * @brief 以下枚举量对应的是Android框架下的MotionEvent类内的"BUTTON_"前缀的常量定义
   */
  enum class Button : int64_t {
    kPrimary = 1 << 0,
    kSecondary = 1 << 1,
    kTertiary = 1 << 2,
  };

  /**
   * @brief 鼠标按键状态，取值为Button的组合
   */
  using ButtonState = int64_t;

  /**
   * @brief 以下枚举量对应的是Android框架下的MotionEvent类内的"TOOL_TYPE_"前缀的常量定义
   */
  enum class ToolType : int64_t {
    kUnknown = 0,
    kFinger = 1,
    kStylus = 2,
    kMouse = 3,
  };

  /**
   * @note 以下字段的声明顺序与类型，需与Java侧序列化数据的内存布局结构一致，见TDFPlatformEvnetHandler.java文件
   */
  struct alignas(8) Pointer {
    int64_t timestamp;
    int64_t pointer_id;
    Action action;
    ButtonState button_state;
    ToolType tool_type;
    AndroidKeyEvent::MetaState meta_state;
    double position_x;
    double position_y;
    double pressure;
    double pressure_min;
    double pressure_max;
    double scroll_delta_x;
    double scroll_delta_y;
  };

  std::vector<Pointer> pointers;

  /**
   * @brief 将数据反序列化，生成AndroidMotionEvent实例
   */
  static AndroidMotionEvent MakeFromData(const uint8_t *data, size_t size);
};

/**
 * @brief 将AndroidKeyEvent::Action转换到KeyboardEvent::EventType
 */
KeyboardEvent::EventType GetKeyboardEventTypeFromAndroidKeyEventAction(AndroidKeyEvent::Action action);

/**
 * @brief 将Android平台的按键码转换到KeyboardKey
 */
KeyboardKey GetKeyboardKeyFromAndroidKeyCode(int64_t key_code);

/**
 * @brief 将AndroidKeyEvent::MetaState转换到ModifierFlags
 */
ModifierFlags GetModifierFlagsFromAndroidKeyEventMetaState(AndroidKeyEvent::MetaState state);

/**
 * @brief 将AndroidKeyEvent的按键字符转换到std::string
 */
std::string GetCharacterFromAndroidKeyEventCharacter(int64_t character);

/**
 * @brief 将AndroidMotionEvent::Action转换到PointerType
 */
PointerType GetPointerTypeFromAndroidMotionEventAction(AndroidMotionEvent::Action action);

/**
 * @brief 将AndroidMotionEvent::Action转换到SignalKind
 */
SignalKind GetSignalKindFromAndroidMotionEventAction(AndroidMotionEvent::Action action);

/**
 * @brief 将AndroidMotionEvent::Button转换到PointerButtonType
 */
PointerButtonType GetPointerButtonsFromAndroidMotionEventButtonState(AndroidMotionEvent::ButtonState state);

/**
 * @brief 将AndroidMotionEvent::ToolType转换到DeviceKind
 */
DeviceKind GetDeviceKindFromAndroidMotionEventToolType(AndroidMotionEvent::ToolType type);

}  // namespace tdfcore
