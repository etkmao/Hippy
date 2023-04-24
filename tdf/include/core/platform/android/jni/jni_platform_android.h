//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <jni.h>

namespace tdfcore {

/**
 * 动态注册JNI方法
 * @param env JNI环境指针
 * @return 是否注册成功
 */
bool Register(JNIEnv *env);

}  // namespace tdfcore
