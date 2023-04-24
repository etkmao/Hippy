//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>
#import <Photos/Photos.h>
#import <MobileCoreServices/MobileCoreServices.h>
#include "core/platform/common/file_picker.h"
@class TDFFilePickerPlugin;
namespace tdfcore {

class FilePickerIOS final : public FilePicker {
 public:
  void Init() override;
  void PickFiles(const PickFileCallback &callback, FileType file_type = FileType::kAny, bool allow_multiple = false,
                   bool with_data = false) override;
  void PickDirectoryPath(const PickDirectoryPathCallback &callback) override;

 private:
  void PickFiles(const std::u16string &file_type, bool allow_multiple, bool with_data);
  void PickDirectoryPath();
  TDFFilePickerPlugin *filePickerPlugin_;
};

}  // namespace tdfcore


@interface TDFFilePickerPlugin : NSObject<UIAdaptivePresentationControllerDelegate, UIDocumentPickerDelegate, UITabBarDelegate, UINavigationControllerDelegate, UIImagePickerControllerDelegate, MPMediaPickerControllerDelegate>

/// 文件选择器-文件选取实现
- (void)pickFile:(tdfcore::FilePicker::PickFileCallback)callback withFileType:(NSString *)fileType allowMultiple:(BOOL)allowMultiple withData:(BOOL)withData;

/// 文件选择器-目录选择实现
- (void)pickDirectory:(tdfcore::FilePicker::PickDirectoryPathCallback)calback;

@end
