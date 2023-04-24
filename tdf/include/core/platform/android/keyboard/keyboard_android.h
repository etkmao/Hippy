//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/common/keyboard/keyboard.h"

namespace tdfcore {

/**
 * @brief Android键盘能力实现类
 */
class KeyboardAndroid final : public Keyboard {
 public:
  explicit KeyboardAndroid(JavaWeakGlobalRef<jobject> df_keyboard_weak_ref);

  ~KeyboardAndroid();

  void Init() override;
/**
 * @brief 弹起键盘
 */
  void Show() override;

  /**
   * @brief 隐藏键盘
   */
  void Hide() override;

  /**
   * @brief 设置编辑信息
   * @param editing_stat 编辑信息e
   */
  void SetEditingState(const EditingState &editing_state) override;

  /**
   * @brief 设置键盘配置和当前键盘使用者的id，内核用client id来记录当前操作键盘的组件标识
   * @param client_id 记录当前键盘使用者的标识
   * @param keyboard_configuration 键盘配置
   */
  void SetKeyboardConfiguration(int32_t id,
                                const KeyboardConfiguration &keyboard_configuration) override;

  void SetCaretRect(const TRect &caret_rect) override {
    caret_rect_ = caret_rect;
  }
  /**
   * @brief 更新编辑信息
   * @param client_id client id
   * @param editing_state 编辑信息
   */
  void UpdateEditingState(int32_t client_id, const EditingState &editing_state);

  /**
   * @brief 键盘是否获取焦点
   * @return
   */
  bool IsShowing() override;

  /***
   * @brief 键盘焦点变化回调
   */
  void OnFocusChanged(bool has_focus);

  /**
   * @brief 注册 JNI 方法
   */
  static bool BindApi(JNIEnv *env);

 private:
  JavaWeakGlobalRef<jobject> df_keyboard_weak_ref_;

  /**
   * @brief JNI 调用，更新编辑信息
   */
  static void NativeUpdateEditingState(JNIEnv *env,
                                       jobject caller,
                                       jlong native_keyboard,
                                       jint client_id, jstring text,
                                       jint selection_start,
                                       jint selection_end,
                                       jint composing_start,
                                       jint composing_end);

  /**
   * @brief JNI 调用，键盘事件回调
   */
  static void NativeOnKeyboardAction(JNIEnv *env,
                                     jobject caller,
                                     jlong native_keyboard,
                                     jint client_id, jint action);

  /**
   * @brief JNI 调用，插入文本
   */
  static void NativeInsertText(JNIEnv *env,
                               jobject caller,
                               jlong native_keyboard,
                               jint client_id, jstring text);

  /**
   * @brief JNI 调用，焦点变化回调
   */
  static void NativeOnFocusChanged(JNIEnv *env,
                                   jobject caller,
                                   jlong native_keyboard,
                                   jboolean has_focus);
};
}  // namespace tdfcore
