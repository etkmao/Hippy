//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <emscripten/val.h>
#include <emscripten/bind.h>
#include "core/platform/common/file_picker.h"

namespace tdfcore {

class FilePickerWeb final : public FilePicker {
 public:
  FilePickerWeb();

  void PickFiles(const PickFileCallback &callback, FileType file_type = FileType::kAny, bool allow_multiple = false,
                 bool with_data = false) override;

  void PickDirectoryPath(const PickDirectoryPathCallback &callback) override;

  PickFileCallback GetPickFileCallback() const { return pick_file_callback_; }

  PickDirectoryPathCallback GetPickDirectoryPathCallback() const { return pick_directory_path_callback_; }

  static void OnPickFilesFinish(intptr_t pointer, const emscripten::val &file_infos_val, bool with_data);

  static void OnPickDirectoryPathFinish(intptr_t pointer, const emscripten::val &directory_path_val);

 private:
  emscripten::val file_picker_ = emscripten::val::null();
  PickFileCallback pick_file_callback_;
  PickDirectoryPathCallback pick_directory_path_callback_;
};
}  // namespace tdfcore
