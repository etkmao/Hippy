//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#import <Foundation/Foundation.h>
#if TARGET_OS_IOS
#include "core/platform/darwin/ios/view/EmbeddedHostView.h"
#elif TARGET_OS_OSX
#include "core/platform/darwin/macos/view/EmbeddedHostView.h"
#endif


@class ChannelRegistry;
typedef EmbeddedHostView * (^CreateEmbeddedViewBlock)(const std::map<std::string, std::string>&, ChannelRegistry*);

@interface EmbeddedViewRegister : NSObject
/*
 * 注册EmbeddedView接口
 * @param viewType 上层调用的EmbeddedView的key角色
 * @param hostViewClass viewType 对应的EmbeddedView类型，注：该类必须继承EmbedHostView
 */
- (void)registerEmbeddedViewWithViewType:(NSString *)viewType viewClass:(Class)hostViewClass;

- (Class)hostViewClassWithViewType:(NSString *)viewType;

- (void)registerEmbeddedViewWithViewType:(NSString *)viewType createBlock:(CreateEmbeddedViewBlock)createBlock;

- (CreateEmbeddedViewBlock)createEmbeddedViewBlockWithViewType:(NSString *)viewType;

@end

