//
//  QLHippyJSAPIBridgeDelegate.h
//  VBTencentVideo
//
//  Created by krisxu on 2022/7/22.
//  Copyright © 2022 Tencent Inc. All rights reserved.
//

#ifndef QLHippyJSAPIBridgeDelegate_h
#define QLHippyJSAPIBridgeDelegate_h

@protocol QLHippyJSAPIBridgeDelegate <NSObject>

@optional

- (void)onHandler:(NSString *)handlerName data:(id)params callBack:(id)callBack;

@end

#endif /* QLHippyJSAPIBridgeDelegate_h */
