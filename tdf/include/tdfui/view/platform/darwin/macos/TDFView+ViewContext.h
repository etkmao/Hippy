//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#import "core/platform/darwin/macos/view/TDFView.h"
#include "tdfui/view/view_context.h"

/**
 * @brief 用于包装ViewContext的包装器类
 */
@interface TDFViewContextWrapper : NSObject

/**
 * @brief ViewContext
 */
@property (nonatomic, assign) std::weak_ptr<tdfcore::ViewContext> value;

/**
 * @brief 创建实例
 */
+ (instancetype)wrapperWithValue:(const std::weak_ptr<tdfcore::ViewContext> &)value;

@end

/**
 * @brief TDFView关联ViewContext
 */
@interface TDFView (ViewContext)

/**
 * @brief 与该实例相关联的ViewContext（由TDFViewContextWrapper进行包装）
 */
@property (nonatomic, strong) TDFViewContextWrapper *viewContext;

@end
