/*!
* iOS SDK
*
* Tencent is pleased to support the open source community by making
* Hippy available.
*
* Copyright (C) 2019 THL A29 Limited, a Tencent company.
* All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#import "TestModule.h"
#import <hippy/HippyRootView.h>
#import <hippy/HippyBridge.h>
#import <hippy/HippyBridgeDelegate.h>
#import "AppDelegate.h"
#import "HippyBundleURLProvider.h"
#import "DemoConfigs.h"

@interface TestModule ()<HippyBridgeDelegate>

/// The debug bundle url
@property (nonatomic, strong) NSURL *debugBundleUrl;

@end

@implementation TestModule

HIPPY_EXPORT_MODULE()

- (dispatch_queue_t)methodQueue
{
	return dispatch_get_main_queue();
}

HIPPY_EXPORT_METHOD(debug:(nonnull NSNumber *)instanceId)
{
//#define REMOTEDEBUG

#ifdef REMOTEDEBUG
	AppDelegate *delegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
	UIViewController *nav = delegate.window.rootViewController;
	UIViewController *vc = [[UIViewController alloc] init];
	BOOL isSimulator = NO;
#if TARGET_IPHONE_SIMULATOR
	isSimulator = YES;
#endif
    
    NSURL *url = [NSURL URLWithString:@"your server ip address"];
    HippyBridge *bridge = [[HippyBridge alloc] initWithBundleURL:url moduleProvider:nil launchOptions:nil];
    HippyRootView *rootView = [[HippyRootView alloc] initWithBridge:bridge moduleName:@"your module name" initialProperties:@{@"isSimulator": @(isSimulator) shareOptions:nil delegate:nil];
	rootView.backgroundColor = [UIColor whiteColor];
	rootView.frame = vc.view.bounds;
	[vc.view addSubview:rootView];
    vc.modalPresentationStyle = UIModalPresentationFullScreen;
    [nav presentViewController:vc animated:YES completion:NULL];
#endif
}

HIPPY_EXPORT_METHOD(remoteDebug:(nonnull NSNumber *)instanceId bundleUrl:(nonnull NSString *)bundleUrl)
{
    AppDelegate *delegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    UIViewController *nav = delegate.window.rootViewController;
    UIViewController *vc = [[UIViewController alloc] init];
    BOOL isSimulator = NO;
#if TARGET_IPHONE_SIMULATOR
    isSimulator = YES;
#endif
    NSString *urlString = [[HippyBundleURLProvider sharedInstance] bundleURLString];
    if (bundleUrl.length > 0) {
        urlString = bundleUrl;
    }

    NSURL *url = [NSURL URLWithString:urlString];
    self.debugBundleUrl = url;
    NSDictionary *launchOptions = @{@"EnableTurbo": @(DEMO_ENABLE_TURBO), @"DebugMode": @(YES)};
    HippyBridge *bridge = [[HippyBridge alloc] initWithDelegate:self 
                                                      bundleURL:url
                                                 moduleProvider:nil
                                                  launchOptions:launchOptions
                                                    executorKey:@"Demo"];
    HippyRootView *rootView = [[HippyRootView alloc] initWithBridge:bridge 
                                                         moduleName:@"Demo"
                                                  initialProperties:@{@"isSimulator": @(isSimulator)}
                                                       shareOptions:nil
                                                           delegate:nil];
    rootView.backgroundColor = [UIColor whiteColor];
    rootView.frame = vc.view.bounds;
    [vc.view addSubview:rootView];
    vc.modalPresentationStyle = UIModalPresentationPageSheet;
    [nav presentViewController:vc animated:YES completion:NULL];
}

- (BOOL)shouldStartInspector:(HippyBridge *)bridge {
    return bridge.debugMode;
}

- (NSURL *)inspectorSourceURLForBridge:(HippyBridge *)bridge {
    return self.debugBundleUrl;
}

#pragma mark - Hippy Bridge Delegate

- (BOOL)shouldUseViewWillTransitionMethodToMonitorOrientation {
    // 是否使用viewWillTransitionToSize方法替换UIApplicationDidChangeStatusBarOrientationNotification通知，
    // 推荐设置 (该系统通知已废弃，且部分场景下存在异常)
    // 注意，设置后必须实现viewWillTransitionToSize方法，并调用onHostControllerTransitionedToSize向hippy同步事件。
    return NO;
}

@end
