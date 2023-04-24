//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#import <UIKit/UIKit.h>

/**
 * @brief 该通知将在UIApplication发送UIEvent时产生，且携带有userInfo
 */
extern NSString *const TDFUIApplicationDidSendEventNotification;

/**
 * @brief 此为TDFUIApplicationDidSendEventNotification的userInfo中的字段之一，其value为UIEvent对象
 */
extern NSString *const TDFUIEventKey;
