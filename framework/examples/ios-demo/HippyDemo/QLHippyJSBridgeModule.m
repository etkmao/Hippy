//
//  QLHippyJSBridgeModule.m
//  TencentVideo
//
//  Created by krisxu on 2022/6/16.
//  Copyright © 2022 Tencent Inc. All rights reserved.
//

#import "QLHippyJSBridgeModule.h"
#import "QLHippyJSAPIBridgeDelegate.h"
#import <hippy/HippyDefines.h>
#import <os/signpost.h>

extern clock_t gXXXBaseTime;

// TODO:
// 第2阶段耗时区间：
//   action:search:end 后
//   开始：第2个 insertBefore const nodeId
//   结束：TimerModule callback
int gStepState = 0;
int gInsertLogCnt = 0;
os_log_t gSignpost;
os_signpost_id_t gSignpostID;

void InitSignpostVars() {
    gSignpost = os_log_create("com.openhippy.app", "MyCategory");
    gSignpostID = os_signpost_id_generate(gSignpost);
}

void SignpostBegin() {
    os_signpost_interval_begin(gSignpost, gSignpostID, "Step2");
}

void SignpostEnd() {
    os_signpost_interval_end(gSignpost, gSignpostID, "Step2");
}

@interface QLHippyJSBridgeModule ()<QLHippyJSAPIBridgeDelegate>

@end

@implementation QLHippyJSBridgeModule

- (instancetype)init
{
    self = [super init];
    if (self) {
        InitSignpostVars();
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
            
//            gStepState = 1;
        }
        
//        if (gStepState == 1) {
//            if([content containsString:@"insertBefore const nodeId"]) {
//                ++gInsertLogCnt;
//            }
//            if (gInsertLogCnt == 2) {
//                gStepState = 2;
//                // step start
//                SignpostBegin();
//            }
//        }
        
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
