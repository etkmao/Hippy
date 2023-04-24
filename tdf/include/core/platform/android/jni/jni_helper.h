//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <jni.h>
#include <any>
#include <map>
#include <string>
#include <vector>
#include "core/common/rect.h"
#include "core/platform/common/view/mutators_stack.h"
#include "core/platform/common/viewport_metrics.h"

namespace tdfcore {

/**
 * @brief JNI初始化，主要是缓存JavaVM对象
 * @param vm JVM对象
 */
jint InitWithJavaVM(JavaVM *vm);

/**
 * @brief 获取当前线程的JNIEnv指针
 * @return JNIEnv指针
 */
JNIEnv *AttachCurrentThread();

/**
 * @brief 解绑jvm和当前线程的绑定关系
 * @return 解绑是否成功
 */
bool DetachCurrentThread();

/**
 * @brief 清除 jni 调用过程中的 Exception
 * @return 是否有异常被清除
 */
bool ClearJEnvException(JNIEnv* env = AttachCurrentThread());

/**
 * @brief 获取jmethodID
 * @param class_name 方法所在的类名字
 * @param method_name 方法名字
 * @param method_sig 方法签名
 * @param env JNIEnv指针
 * @return jmethodID
 */
jmethodID GetMethodID(const char *class_name, const char *method_name, const char *method_sig,
                      JNIEnv *env = nullptr);

/**
 * @brief 获取jmethodID
 * @param clazz 方法所在的jclass类
 * @param method_name 方法名字
 * @param method_sig 方法签名
 * @param env JNIEnv指针
 * @return jmethodID
 */
jmethodID GetMethodID(jclass clazz, const char *method_name, const char *method_sig,
                      JNIEnv *env = nullptr);
/**
 * @brief 调用对象的返回类型为jobject的方法
 * @param obj 被调用对象
 * @param method_id 调用方法id
 * @param args 方法参数
 * @return jobject对象
 */
jobject CallObjectMethod(jobject obj, jmethodID method_id, va_list args);

/**
 * @brief 调用对象的返回类型为jobject的方法
 * @param obj 被调用对象
 * @param method_id 调用方法id
 * @param ... 方法参数
 * @return jobject对象
 */
jobject CallObjectMethod(jobject obj, jmethodID method_id, ...);

/**
 * @brief 调用对象的返回类型为void的方法
 * @param obj 被调用对象
 * @param method_id 调用方法id
 * @param args 方法参数
 */
void CallVoidMethod(jobject obj, jmethodID method_id, va_list args);

/**
 * @brief 调用对象的返回类型为void的方法
 * @param obj 被调用对象
 * @param method_id 调用方法id
 * @param ... 方法参数
 */
void CallVoidMethod(jobject obj, jmethodID method_id, ...);

/**
 * @brief 调用对象的返回类型为int的方法
 * @param obj 被调用对象
 * @param method_id 调用方法id
 * @param ... 方法参数
 * @return jint类型
 */
jint CallIntMethod(jobject obj, jmethodID method_id, va_list args);

/**
 * @brief 调用对象的返回类型为int的方法
 * @param obj 被调用对象
 * @param method_id 调用方法id
 * @param ... 方法参数
 * @return jint类型
 */
jint CallIntMethod(jobject obj, jmethodID method_id, ...);

/**
 * @brief 调用对象的返回类型为long的方法
 * @param obj 被调用对象
 * @param method_id 调用方法id
 * @param ... 方法参数
 * @return jlong类型
 */
jlong CallLongMethod(jobject obj, jmethodID method_id, va_list args);

/**
 * @brief 调用对象的返回类型为long的方法
 * @param obj 被调用对象
 * @param method_id 调用方法id
 * @param ... 方法参数
 * @return jlong类型
 */
jlong CallLongMethod(jobject obj, jmethodID method_id, ...);


/**
 * @brief 调用对象的返回类型为boolean的方法
 * @param obj 被调用对象
 * @param method_id 调用方法id
 * @param ... 方法参数
 * @return jboolean 类型
 */
jboolean CallBooleanMethod(jobject obj, jmethodID method_id, va_list args);

/**
 * @brief 调用对象的返回类型为boolean的方法
 * @param obj 被调用对象
 * @param method_id 调用方法id
 * @param ... 方法参数
 * @return jboolean 类型
 */
jboolean CallBooleanMethod(jobject obj, jmethodID method_id, ...);

/**
 * @brief 查找jclass对象
 * @param class_name 想要查找的类的全限定名称
 * @param env JNIEnv指针
 * @return jclass
 */
jclass FindClass(const char *class_name, JNIEnv *env = nullptr);

/**
 * @brief char指针转jstring对象
 * @param str char指针
 * @param env JNIEnv指针
 * @return jstring对象
 */
jstring Char2JString(const char *str, JNIEnv *env = AttachCurrentThread());

/**
 * @brief jstring对象转为char指针
 * @param j_str jstring对象
 * @param env JNIEnv指针
 * @return char指针
 */
std::string JString2String(jstring j_str, JNIEnv *env = AttachCurrentThread());

/**
 * @brief jstring对象转为u16string对象
 * @param j_str jstring对象
 * @param env JNIEnv指针
 * @return u16string对象
 */
std::u16string JString2U16string(jstring j_str, JNIEnv *env = AttachCurrentThread());

/**
 * @brief jstring对象转为std::string
 * @param j_str jstring对象
 * @param env JNIEnv指针
 * @return std::string
 */
std::string JString2UTF8String(jstring j_str, JNIEnv *env = AttachCurrentThread());

/**
 * @brief u16string对象转为jstring对象
 * @param u16 u16对象
 * @param env JNIEnv指针
 * @return jstring对象
 */
jstring U16string2JString(std::u16string u16, JNIEnv *env = AttachCurrentThread());

/**
 * @brief key,value为string类型的map转位Java层的HashMap<String, String>
 * @param map key,value为string类型的map
 * @param env JNIEnv指针
 * @return 转后的HashMap<String, String>对象
 */
jobject StringMap2JStringMap(const std::map<std::string, std::string> &map,
                             JNIEnv *env = AttachCurrentThread());

/**
 * @brief 创建代码Java对象的jobject
 * @param clazz 对象所属的jclass对象
 * @param method_name 构造函数字符串
 * @param method_sig 方法签名
 * @param ... 参数列表
 * @return 创建完的jobject
 */
jobject NewObject(jclass clazz, const char *method_name, const char *method_sig, ...);

/**
 * @brief 创建代码Java对象的jobject
 * @param class_name 对象所属的jclass的全限定名称
 * @param method_name 构造函数字符串
 * @param method_sig 方法签名
 * @param ... 参数列表
 * @return 创建完的jobject
 */
jobject NewObject(const char *class_name, const char *method_name, const char *method_sig, ...);

/**
 * @brief 调用jclass中的返回类型为jstring的静态方法
 * @param clazz 被调用的jclass独享
 * @param method_name 被调用的静态方法
 * @param method_sig 方法签名
 * @param ... 参数列表
 * @return 返回的jstring对象
 */
jstring CallStaticJStringMethod(jclass clazz, const char *method_name, const char *method_sig, ...);

std::vector<double> JDoubleArray2DoubleArray(jdoubleArray jdouble_array, JNIEnv *env = AttachCurrentThread());
}  // namespace tdfcore
