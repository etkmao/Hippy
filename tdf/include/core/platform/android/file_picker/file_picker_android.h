//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/common/file_picker.h"

namespace tdfcore {

/***
 * @brief Android侧文件选择类实现
 */
class FilePickerAndroid : public FilePicker {
 public:
  explicit FilePickerAndroid(JavaWeakGlobalRef<jobject> file_picker_ref);

  ~FilePickerAndroid();

  void Init() override;

  void PickFiles(const PickFileCallback &callback, FileType file_type = FileType::kAny, bool allow_multiple = false,
                 bool with_data = false) override;
  void PickDirectoryPath(const PickDirectoryPathCallback &callback) override;
  /***
   * @brief 获取文件选择回调函数
   * @return
   */
  PickFileCallback GetFilePickerCallback() const { return pick_file_callback_; }

  /***
   * @brief 获取目录选择回调函数
   * @return
   */
  PickDirectoryPathCallback GetDirectoryPathPickCallback() const { return pick_directory_path_callback_; }

  /**
  * @brief 注册 JNI 方法
  */
  static bool BindApi(JNIEnv *env);

 private:
  JavaWeakGlobalRef<jobject> file_picker_weak_ref_;

  /**
   * @brief 文件选择回调
   */
  static void OnFilePicked(JNIEnv *env,
                           jobject caller,
                           jlong native_file_picker,
                           jobject file_info_list_object);

  /**
   * @brief 路径选择回调
   */
  static void OnDirectoryPathPicked(JNIEnv *env,
                                    jobject caller,
                                    jlong native_file_picker,
                                    jstring dir_path);
};

}  // namespace tdfcore
