//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#import <Foundation/Foundation.h>
#import "core/platform/darwin/macos/view/TDFView.h"
#include "core/tdfi/shell.h"
#include "core/platform/darwin/macos/platform_macos.h"
#import "core/platform/darwin/common/TDFEngineConfig.h"
#import "core/platform/darwin/common/EmbeddedViewRegister.h"
#import "core/platform/darwin/common/channel/channel_runtime_darwin.h"

NS_ASSUME_NONNULL_BEGIN

@interface TDFEngine : NSObject

@property (nonatomic, strong, readonly) TDFView *view;
@property (nonatomic, strong, readonly) EmbeddedViewRegister *embeddedViewRegister;

- (instancetype)initWithConfiguration:(TDFEngineConfig *)config NS_DESIGNATED_INITIALIZER;

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

- (std::shared_ptr<tdfcore::Shell>)getShell;
- (tdfcore::PlatformMacOS *)getPlatformMacOS;
- (ChannelRegistry *)getChannelRegistry;

/// @brief 启动内核Pipeline
- (void)start;

/// @brief 停止内核Pipeline
- (void)pause;

/// @brief 重新恢复内核Pipeline
- (void)resume;

/// @brief 终止内核Pipeline
- (void)terminate;

/// @brief 通知内核应用处于不可见状态
- (void)onAppPause;

/// @brief 通知内核应用处于可见状态
- (void)onAppResume;

/// @brief 通知内核应用处于不可交互状态
- (void)onAppInactive;

/// @brief 通知内核应用即将销毁
- (void)onAppDestroy;

@end

NS_ASSUME_NONNULL_END
