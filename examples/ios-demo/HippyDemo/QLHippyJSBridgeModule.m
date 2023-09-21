//
//  QLHippyJSBridgeModule.m
//  TencentVideo
//
//  Created by krisxu on 2022/6/16.
//  Copyright © 2022 Tencent Inc. All rights reserved.
//

#import "QLHippyJSBridgeModule.h"
#import "QLHippyJSAPIBridgeDelegate.h"

@interface QLHippyJSBridgeModule ()<QLHippyJSAPIBridgeDelegate>

@end

@implementation QLHippyJSBridgeModule

- (instancetype)init
{
    self = [super init];
    if (self) {

    }
    return self;
}

HIPPY_EXPORT_MODULE(TencentJsBridgeModule)

HIPPY_EXPORT_METHOD(callJsApi:(NSDictionary *)dic resolver:(HippyPromiseResolveBlock)resolve) {
    NSString *funcName = dic[@"funcName"];
    if (!funcName) {
        resolve(@"funcName cannot be null");
        return;
    }

    if ([funcName isEqualToString:@"writeLog"]) {
        NSString *content = dic[@"content"];
        NSLog(@"[VideoPageLog] - %@", content);
    }
    
    //NSLog(@"[XXVideoPageLog] - %@", dic);
}

#pragma mark - QLHippyJSAPIBridgeDelegate

- (void)onHandler:(NSString *)handlerName data:(id)params callBack:(id)callBack {
    [self sendEvent:handlerName params:params];
}

@end
