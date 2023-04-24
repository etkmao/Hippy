//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#import <MobileCoreServices/MobileCoreServices.h>
#import <Photos/Photos.h>
#include "core/platform/common/file_manager.h"

typedef NS_ENUM(NSInteger, MediaType) {
  kVideo,
  kImage,
  kMedia
};

@interface TDFFileUtils : NSObject
+ (NSArray<NSString*>*) resolveType:(NSString*)type withAllowedExtensions:(NSArray<NSString*>*)allowedExtensions;
+ (MediaType) resolveMediaType:(NSString*)type;
+ (std::vector<tdfcore::FileInfo>) resolveFileInfo:(NSArray<NSURL *> *)urls withData:(BOOL)loadData;
+ (NSURL*) exportMusicAsset:(NSString*)url withName: (NSString*)name;
+ (BOOL)hasAlpha:(UIImage *)image;
+ (NSURL*)saveTmpImage:(UIImage *)image;
@end



