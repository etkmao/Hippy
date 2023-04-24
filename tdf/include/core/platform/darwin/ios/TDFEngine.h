//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "core/platform/darwin/ios/view/TDFView.h"
#include "core/tdfi/shell.h"
#include "core/platform/darwin/ios/platform_ios.h"
#import "core/platform/darwin/common/channel/channel_runtime_darwin.h"
#import "core/platform/darwin/common/TDFEngineConfig.h"
#import "core/platform/darwin/common/EmbeddedViewRegister.h"

@interface TDFEngine : NSObject

@property(nonatomic, strong, readonly) TDFView *view;
@property(nonatomic, strong, readonly) EmbeddedViewRegister *embeddedViewRegister;

- (instancetype)initWithConfiguration:(TDFEngineConfig *)config NS_DESIGNATED_INITIALIZER;

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

- (std::shared_ptr<tdfcore::Shell>)getShell;
- (tdfcore::PlatformIOS *)getPlatformIOS;
- (ChannelRegistry *)getChannelRegistry;

/// @brief 启动内核Pipeline
- (void)start;

/// @brief 停止内核Pipeline
- (void)pause;

/// @brief 重新恢复内核Pipeline
- (void)resume;

/// @brief 终止内核Pipeline
- (void)terminate;

/// @brief 通知内核内存告警
- (void)didReceiveMemoryWarning;

@end
