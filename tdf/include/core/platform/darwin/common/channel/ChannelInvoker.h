//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#include "core/platform/common/channel/native_value_wrapper.h"

@interface ChannelInvoker : NSObject

+ (void) channelSetupMethods:(id)channel;
+ (std::shared_ptr<tdfcore::NativeValueWrapper>) invokeObjCMethod:(NSString *)methodName
                                                      argumentCount:(size_t)argumentCount
                                                          arguments:(const tdfcore::NativeValueWrapper [])arguments
                                                             object:(id)obj;
+ (id) invokeCoreMethod:(NSString *)channel
                 method:(NSString *)method
              arguments:(NSArray *)arguments;

+ (void) setMethodHandler:(NSString *)channel
                 callback:(const tdfcore::MethodHandler &)handler;

+ (void) removeMethodHandler:(NSString *)channel;

@end


@interface ChannelMethod : NSObject

- (instancetype) initWithMethodSignature:(NSString *)objCMethodName
                              methodName:(NSString *)methodName
                             moduleClass:(Class)moduleClass;
- (id) invokeMethod:(NSString *)methodName obj:(id)obj arguments:(const tdfcore::NativeValueWrapper [])arguments;

@end

@interface MethodHandler : NSObject

- (instancetype) initWithMethodHandler:(const tdfcore::MethodHandler &)methodHadler;

- (id) invokeCoreMethod:(NSString *)methodName
              arguments:(NSArray *)arguments;

@end
