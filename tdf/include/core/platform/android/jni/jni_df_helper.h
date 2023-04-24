//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/platform/android/jni/jni_helper.h"

namespace tdfcore {

/**
 * @brief 回调Java层的DynamicFrameworkJNI类中的返回类型为void方法
 * @param df_obj DynamicFrameworkJNI对象对应的jobject
 * @param method_name 方法名字
 * @param method_sig 方法签名
 * @param ... 方法参数
 */
void CallDFVoidMethod(jobject df_obj, const char *method_name, const char *method_sig, ...);

/**
 * @brief 回调Java层的DynamicFrameworkJNI类中返回类型为int的方法
 * @param df_obj DynamicFrameworkJNI对象对应的jobject
 * @param method_name 方法名字
 * @param method_sig 方法签名
 * @param ... 方法参数
 */
jint CallDFIntMethod(jobject df_obj, const char *method_name, const char *method_sig, ...);

/**
 * @brief 回调Java层的DynamicFrameworkJNI类中返回类型为bool的方法
 * @param df_obj DynamicFrameworkJNI对象对应的jobject
 * @param method_name 方法名字
 * @param method_sig 方法签名
 * @param ... 方法参数
 */
jboolean CallDFBoolMethod(jobject df_obj, const char *method_name, const char *method_sig, ...);

/**
 * @brief 回调Java层的DynamicFrameworkJNI类中返回类型为Object的方法
 * @param df_obj DynamicFrameworkJNI对象对应的jobject
 * @param method_name 方法名字
 * @param method_sig 方法签名
 * @param ... 方法参数
 */
jobject CallDFObjectMethod(jobject df_obj, const char *method_name, const char *method_sig, ...);

/**
 * @brief 初始化需要的jclass对象，用于后续的JNI方法回调
 */
void InitClazz();

/**
 * @brief 获取Java层DynamicFrameworkJNI对应的jclass对象
 * @return DynamicFrameworkJNI对应的jclass对象
 */
jclass GetTDFJniClazz();

/**
 * @brief 获取Java层KeyboardConfigInputType对应的jclass对象
 * @return KeyboardConfigInputType对应的jclass对象
 */
jclass GetKeyboardConfigInputTypeClazz();

/**
 * @brief 获取Java层KeyboardConfig对应的jclass对象
 * @return KeyboardConfig对应的jclass对象
 */
jclass GetKeyboardConfigClazz();

/**
 * @brief 获取Java层KeyboardEditingState对应的jclass对象
 * @return KeyboardEditingState对应的jclass对象
 */
jclass GetKeyboardEditingStateClazz();

/**
 * @brief 获取Java层KeyboardChannnel对应的jclass对象
 * @return KeyboardEditingState对应的jclass对象
 */
jclass GetKeyboardChannelClazz();

/**
 * @brief 获取Java层ChannelRegistry对应的jclass对象
 * @return ChannelRegistry对应的jclass对象
 */
jclass GetChannelRegistryClazz();

/***
 * @brief 获取Java侧FilePickerChannel对应的Class反射类
 * @return
 */
jclass GetFilePickerChannelClazz();

/***
 * @brief 获取Java侧PlatformEventHandler对应的Class反射类
 */
jclass GetPlatformEventHandlerClazz();

// 下面三个方法主要是为了管理C++ JNI对象生命周期，Java侧通过handle指针找
// 对应的C++对象时，统一通过这里的map容器查找，避免对象释放Java侧感知不到Crash问题
void AddJNIObject(int64_t id, const std::weak_ptr<Object> &object);
bool RemoveJNIObject(int64_t id);
std::shared_ptr<Object> GetJNIObject(int64_t id);

}  // namespace tdfcore
