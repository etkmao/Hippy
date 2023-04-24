//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#import <UIKit/UIKit.h>
#include <memory.h>
#import "core/platform/darwin/common/TDFEngineConfig.h"
#import "core/platform/common/view/platform_view.h"
#import "core/platform/darwin/ios/event/TDFPlatformEventHandler.h"

@interface TDFView : UIView

@property(nonatomic) std::weak_ptr<tdfcore::PlatformView> platform_view;

/**
 * @brief 获取Layer
 * self.layer不能在子线程访问，子线程访问使用该接口替代
 */
- (CALayer *)getLayerSafety;

@end

/// TDF渲染视图
@interface TDFOutputView : TDFView

/**
 * @brief 平台事件处理器
 */
@property(nonatomic, strong) TDFPlatformEventHandler *eventHandler;

@end

/// TDF EmbeddedView覆盖视图
@interface TDFOverlayView : TDFView

@property(nonatomic, assign) NSInteger viewID;

/**
 * @brief 初始化
 * @param viewID ID
 */
- (instancetype)initWithViewID:(NSInteger)viewID;

@end

