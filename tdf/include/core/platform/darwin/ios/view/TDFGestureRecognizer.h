#import <UIKit/UIKit.h>
#import "core/platform/darwin/ios/event/TDFPlatformEventHandler.h"

/**
 * @brief 用于从TDFView上接收触摸事件
 */
@interface TDFGestureRecognizer : UIGestureRecognizer

/**
 * @brief 获取所有的触摸点信息
 */
- (NSSet<UITouch *> *)allTouches;

/**
 * @brief 主动令手势失败
 */
- (void)cancel;

@end

/**
 * @brief 用于TDFView内所有手势的创建与管理
 */
@interface TDFGestureRecognizerManager : NSObject

/**
 * @brief 平台事件处理器
 */
@property(nonatomic, strong) TDFPlatformEventHandler *eventHandler;

/**
 * @brief 将手势添加到指定的View上
 */
- (void)attachToView:(UIView *)view;

/**
 * @brief 将手势从原先的View上移除
 */
- (void)detach;

@end
