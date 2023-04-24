//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import "core/tdfi/event.h"
#import "core/tdfi/shell.h"

/**
 * @brief 处理平台侧系统事件
 */
@interface TDFPlatformEventHandler : NSObject

/**
 * @brief 初始化
 * @param shell 本类会对平台侧系统事件进行转化处理，生成TDF统一定义的事件，给到指定的Shell实例做进一步的处理
 */
- (instancetype)initWithShell:(const std::weak_ptr<tdfcore::Shell> &)shell;

/**
 * @brief 处理系统事件
 * @param event 平台侧系统事件对象，不可为nil
 * @param view 该event的原始响应者NSView对象；若无，请填nil，将认为该event的原始响应者为其当前的NSWindow对象
 * @return 是否处理了该事件
 */
- (BOOL)handleEvent:(NSEvent *)event ofView:(NSView *)view;

/**
 * @brief 处理视窗尺寸变更事件
 * @param view 渲染输出的目标NSView
 */
- (void)handleOutputViewFrameChanged:(NSView *)view;

/**
 * @brief 处理文件拖拽事件
 * @param info 拖拽信息
 * @param view 发生文件拖拽的NSView对象
 * @param state 当前的拖拽状态
 */
- (void)handleFileDragging:(id<NSDraggingInfo>)info ofView:(NSView *)view state:(tdfcore::FileDragEvent::DragState)state;

@end
