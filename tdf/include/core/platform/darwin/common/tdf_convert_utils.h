//
//  tdf_convert_utils.h
//  core
//
//
#pragma once

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

#include <string>

#include "core/platform/common/keyboard/keyboard_constants.h"

NS_ASSUME_NONNULL_BEGIN

// OC类型与C++类型之间转换工具类
@interface TDFConvertUtils : NSObject

// NSString 转 UTF16 String
+ (std::u16string)NSStringToUTF16:(NSString *)string;

// UTF16 String 转 NSString
+ (NSString *)UTF16ToNSString:(const std::u16string &)string;

// 转换为内核ModifierFlags
#if TARGET_OS_IPHONE
+ (ModifierFlags)convertToModifierFlags:(UIKeyModifierFlags)modifierFlags;
#else
+ (ModifierFlags)convertToModifierFlags:(NSEventModifierFlags)modifierFlags;
#endif


@end

NS_ASSUME_NONNULL_END
