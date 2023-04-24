//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#import <Cocoa/Cocoa.h>
#include <map>
#include <string>
#include <vector>
#include "core/platform/common/view//platform_embedded_view.h"
#include "core/platform/common/view/mutators_stack.h"
#include "core/support/gesture/recognizer/gesture_recognizer.h"
#include "core/common/rect.h"
#include "core/common/bitmap.h"

NS_ASSUME_NONNULL_BEGIN
@interface EmbeddedHostView : NSView
/*
 初始化方法，子类可重写
 */
- (instancetype)initWithPropsMap:(const std::map<std::string, std::string> &)propsMap;

/*
 属性更新时调用，子类可重写
*/
- (void)updateWithProps:(const std::map<std::string, std::string> &)props;

/*
 hostView自身位置发生变化后回调用，子类可重写
*/
- (void)layoutDidChangedWithFrame:(NSRect)frame;

- (void)updateMutators:(const std::vector<std::shared_ptr<tdfcore::Mutator>> &)mutators;

/**
 * @brief 获取截图
 */
- (std::future<std::shared_ptr<tdfcore::Bitmap>>)makeSnapshot;

/*
 资源释放回调，子类可以在此方法释放EmbeddedView的资源
*/
- (void)onDispose;

@end

NS_ASSUME_NONNULL_END
