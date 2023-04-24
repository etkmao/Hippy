//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <jni.h>

#include "core/platform/android/jni/jni_helper.h"
#include "core/common/macro.h"

namespace tdfcore {

/**
 * @brief JNI引用包装基类
 */
template <typename T>
class JavaRef {
 public:
  explicit JavaRef(T obj) : obj_(obj) {}

  JavaRef() = default;

  ~JavaRef() = default;

  /**
   * @brief 当前引用对应的Obj是否失效
   * @return 是否失效
   */
  bool Invalid() const { return this->obj_ == nullptr; }

  /**
   * @brief 获取引用对应的Obj
   * @return 引用对应的Obj
   */
  T Object() const { return static_cast<T>(obj_); }

  TDF_DISALLOW_COPY_AND_ASSIGN(JavaRef);

 protected:
  jobject obj_ = nullptr;
};

/**
 * @brief JNI局部引用类，会在析构函数里面自动Delete掉JNI的局部引用
 */
template <typename T>
class JavaLocalRef : public JavaRef<T> {
 public:
  JavaLocalRef(T obj, JNIEnv *env) : JavaRef<T>(obj), env_(env) {
    this->obj_ = env->NewLocalRef(obj);
  }

  TDF_DISALLOW_IMPLICIT_CONSTRUCTORS(JavaLocalRef);

  ~JavaLocalRef() {
    if (this->obj_) {
      this->env_->DeleteLocalRef(this->obj_);
      this->obj_ = nullptr;
    }
  }

 private:
  JNIEnv *env_;
};

/**
 * @brief JNI全局引用类，会在析构函数里面自动Delete掉JNI的全局引用
 */
template <typename T>
class JavaGlobalRef : public JavaRef<T> {
 public:
  explicit JavaGlobalRef(T obj) : JavaRef<T>(obj) {
    auto env = AttachCurrentThread();
    this->obj_ = env->NewGlobalRef(this->obj_);
  }

  JavaGlobalRef() = delete;

  ~JavaGlobalRef() { this->DeleteGlobalRef(); }

  JavaGlobalRef<T>(const JavaGlobalRef<T> &other) {
    if (&other == this) {
      return;
    }
    auto env = AttachCurrentThread();
    this->obj_ = other.obj_ ? env->NewGlobalRef(other.obj_) : nullptr;
  }

  JavaGlobalRef<T> &operator=(const JavaGlobalRef<T> &rhs) {
    if (&rhs == this) {
      return *this;
    }
    auto env = AttachCurrentThread();
    if (this->obj_) {
      env->DeleteGlobalRef(this->obj_);
    }
    this->obj_ = rhs.obj_ ? env->NewGlobalRef(rhs.obj_) : nullptr;
    return *this;
  }

 private:
  void DeleteGlobalRef() {
    auto env = AttachCurrentThread();
    if (this->obj_) {
      env->DeleteGlobalRef(this->obj_);
      this->obj_ = nullptr;
    }
  }
};

/**
 * @brief JNI全局弱引用类，会在析构函数里面自动Delete掉JNI的全局弱引用
 */
template <typename T>
class JavaWeakGlobalRef : public JavaRef<T> {
 public:
  explicit JavaWeakGlobalRef(T obj) : JavaRef<T>(obj) {
    auto env = AttachCurrentThread();
    this->obj_ = env->NewWeakGlobalRef(obj);
  }

  JavaWeakGlobalRef<T>(const JavaWeakGlobalRef<T> &other) {
    if (&other == this) {
      return;
    }

    auto env = AttachCurrentThread();
    this->obj_ = other.obj_ ? env->NewWeakGlobalRef(other.obj_) : nullptr;
  }

  JavaWeakGlobalRef<T> &operator=(const JavaWeakGlobalRef<T> &rhs) {
    if (&rhs == this) {
      return *this;
    }

    auto env = AttachCurrentThread();
    if (this->obj_) {
      env->DeleteWeakGlobalRef(this->obj_);
    }
    this->obj_ = rhs.obj_ ? env->NewWeakGlobalRef(rhs.obj_) : nullptr;
    return *this;
  }

  ~JavaWeakGlobalRef() {
    if (this->obj_) {
      AttachCurrentThread()->DeleteWeakGlobalRef(this->obj_);
      this->obj_ = nullptr;
    }
  }
};
}  // namespace tdfcore