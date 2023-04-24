//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include "core/platform/common/file_manager.h"
#include <android/asset_manager_jni.h>
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/android/jni/jni_df_helper.h"

namespace tdfcore {
class FileManagerAndroid : public FileManager {
 public:
  FileManagerAndroid(const JavaWeakGlobalRef<jobject> &weak_ref, const jobject &fileinfo_jobject);
  ~FileManagerAndroid() override = default;

  virtual bool FileExists(const std::string &path) const override;

  virtual bool TCreateDirectory(const std::string &path) const override;

  virtual bool TDeleteFile(const std::string &path) const override;

  virtual std::vector<FileInfo> GetFileInfos(const std::string &path) const override;

  virtual std::shared_ptr<TData> GetAssetsFile(const std::string &file_name) const override;

 private:
  tdfcore::JavaWeakGlobalRef<jobject> df_jni_weak_ref_;

  /// java 自定义类jcalss必须在main线程获取
  jobject file_info_jobject;
};
}  // namespace tdfcore
