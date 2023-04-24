//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#import <Cocoa/Cocoa.h>
#import "core/platform/common/view/platform_view.h"
#import "core/platform/darwin/common/TDFEngineConfig.h"
#import "core/platform/darwin/macos/event/TDFPlatformEventHandler.h"
#import "core/tdfi/event.h"

NS_ASSUME_NONNULL_BEGIN

@protocol TDFOutputViewDelegate<NSObject>

/**
 * @brief 是否忽略HitTest
 */
- (BOOL)tdfViewShouldIgnoreWithPoint:(NSPoint)point;

/**
 * @brief 处理排版事件
 */
- (void)layout;

@end

@interface TDFView : NSView

@property(nonatomic) std::weak_ptr<tdfcore::PlatformView> platform_view;

/**
 * @brief 获取Layer
 * 可以在子线程获取
 */
- (CALayer *)getLayerSafety;

@end

/// TDF渲染视图
@interface TDFOutputView : TDFView<NSDraggingDestination>

/// 初始化
/// @param graphicsAPI 图形API
/// @param delegate 代理
- (instancetype)initWithDelegate:(id<TDFOutputViewDelegate>)delegate;

/// 平台事件处理器，用于处理TDFOutputView收到的平台事件
@property(nonatomic, strong) TDFPlatformEventHandler *eventHandler;

@end

/// TDF EmbeddedView覆盖视图
@interface TDFOverlayView : TDFView

@property(nonatomic, assign) NSInteger viewID;

/// 初始化
/// @param graphicsAPI 图形API
/// @param viewID ID
- (instancetype)initWithFrame:(NSRect)frame viewID:(NSInteger)viewID;

@end

NS_ASSUME_NONNULL_END
