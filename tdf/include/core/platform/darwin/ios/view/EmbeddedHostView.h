//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#import <UIKit/UIKit.h>
#include <map>
#include <string>
#include <vector>
#include <future>
#include "core/platform/common/view/platform_embedded_view.h"
#include "core/platform/common/view/mutators_stack.h"
#include "core/platform/darwin/common/channel/ChannelRegistry.h"
#include "core/support/gesture/recognizer/gesture_recognizer.h"
#include "core/common/rect.h"
#include "core/engine/graphics/texture.h"

NS_ASSUME_NONNULL_BEGIN
@interface EmbeddedHostView : UIView

/**
 * @brief 唯一初始化方法，子类可重写
 */
- (instancetype)initWithPropsMap:(const std::map<std::string, std::string> &)properties
                 channelRegistry:(ChannelRegistry *)channelRegistry;

/*
 属性更新时调用，子类可重写
*/
- (void)updateWithProps:(const std::map<std::string, std::string> &)props;

/*
 应用Mutator Transform
*/
- (void)applyMutatorTransform:(CGAffineTransform)transform;

/**
 * @brief 获取截图
 */
- (std::future<std::shared_ptr<tdfcore::Bitmap>>)makeSnapshot;

/*
 资源释放回调，子类可以在此方法释放EmbeddedView的资源
*/
- (void)onDispose;

/*
 资源释放回调，子类可以在此方法释放EmbeddedView的资源
*/
- (void)onDispose;

@end

NS_ASSUME_NONNULL_END
