//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#import <Foundation/Foundation.h>
#include "core/platform/darwin/common/channel/ChannelInvoker.h"

#define CHANNEL(name)                                                                                  \
    - (NSString *)channelName {                                                                        \
        return @ #name;                                                                                \
    }                                                                                                  \
    - (id)invokeMethod:(NSString *)method                                                              \
             arguments:(NSArray *)arguments {                                                          \
        return [ChannelInvoker invokeCoreMethod:[self channelName] method:method arguments:arguments]; \
    }

#define CHANNEL_METHOD(name, method) REGISTER_METHOD(name, method, __LINE__, __COUNTER__)

#define REGISTER_METHOD(name, method, line, count)   \
    EXPORT_METHOD(name, method, line, count)         \
    -(id)method

#define EXPORT_METHOD(name, method, line, count)                     \
    +(NSArray<NSString *> *)__channel_method__##name##line##count {  \
        return @[@ #name, @ #method];                                \
    }


@interface ChannelRegistry : NSObject

- (void)registerChannel:(nullable id)channel;

- (nullable void *)channelWithName:(nullable NSString *)channelName;

@end

