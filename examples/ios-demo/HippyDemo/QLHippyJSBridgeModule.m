//
//  QLHippyJSBridgeModule.m
//  TencentVideo
//
//  Created by krisxu on 2022/6/16.
//  Copyright © 2022 Tencent Inc. All rights reserved.
//

#import "QLHippyJSBridgeModule.h"
#import "QLHippyJSAPIBridgeDelegate.h"

extern clock_t gXXXBaseTime;

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

HIPPY_EXPORT_METHOD(callJsApi:(NSDictionary *)dic resolver:(HippyPromiseResolveBlock)resolve rejector:(HippyPromiseRejectBlock)rejector) {
    NSString *funcName = dic[@"funcName"];
    if (!funcName) {
        resolve(@"funcName cannot be null");
        return;
    }

    if ([funcName isEqualToString:@"writeLog"]) {
        NSString *content = dic[@"content"];
        NSLog(@"[VideoPageLog] - %@", content);
        
        if([content containsString:@"action:search:end"]) {
            gXXXBaseTime = 0;
        }
        
        if([content containsString:@"result took"]) {
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                exit(0);
            });
        }
    }
    
    //NSLog(@"[XXVideoPageLog] - %@", dic);
}

#pragma mark - QLHippyJSAPIBridgeDelegate

- (void)onHandler:(NSString *)handlerName data:(id)params callBack:(id)callBack {
    [self sendEvent:handlerName params:params];
}

@end
