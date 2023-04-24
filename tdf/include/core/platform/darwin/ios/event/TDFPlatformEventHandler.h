//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#import <UIKit/UIKit.h>
#import "core/tdfi/shell.h"

// TODO(dexterzhao): 实现一致性问题
/**
 * @note 不同于其它平台，iOS平台上，各种硬件输入类事件的API通道，在形式上有着不一致性：
 *     触摸类：需要通过手势系统，不能通过覆写UIView自身的touch系列（因为还有其它手势的存在，touch系列方法会收不到回调）
 *     鼠标类：仍旧要通过手势系统，没有鼠标相关的API
 *     键盘类：有两条路可选，方法2要同步修改文字编辑模块，暂时先用方法1
 *     （1）用黑科技hook去替换UIApplication的sendEvent方法，截取所有事件，再过滤出按键相关的事件
 *     （2）对TDFOutputView与文字编辑用到的隐藏View，覆写Focus与FirstResponder，在press系列方法里转发事件
 *     对于TDFPlatformEventHandler，从一致性角度来看，要么所有硬件输入类事件全靠调用者输入，要么自己无侵入式监控到所有事件，再做过滤（这个其实难以做到）。
 *     目前键盘事件跟其它类事件的处理就是不一致
 */

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
 * @brief 处理手势
 * @param gesture 平台侧手势对象，不可为nil
 * @return 是否处理了该手势
 */
- (BOOL)handleGesture:(UIGestureRecognizer *)gesture;

/**
 * @brief 处理视窗尺寸变更事件
 * @param view 渲染输出的目标UIView
 */
- (void)handleOutputViewFrameChanged:(UIView *)view;

/**
 * @brief 处理软键盘尺寸变更事件
 * @param frame 软键盘尺寸
 */
- (void)handleKeyboardFrameChanged:(CGRect)frame;

@end
