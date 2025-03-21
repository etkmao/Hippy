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

#import "HippyJSCExecutor.h"

#import <cinttypes>
#import <memory>
#import <pthread.h>
#import <string>
#import <unordered_map>

#import <UIKit/UIDevice.h>
#import <CoreFoundation/CoreFoundation.h>

#import "HippyAssert.h"
#import "HippyBridge+Private.h"
#import "HippyDefines.h"
#import "HippyDevMenu.h"
#import "HippyDevInfo.h"
#import "HippyJavaScriptLoader.h"
#import "HippyLog.h"
#import "HippyPerformanceLogger.h"
#import "HippyUtils.h"
#import "HippyRedBox.h"
#import "HippyJSCWrapper.h"
#import "HippyJSCErrorHandling.h"
#import "HippyJSEnginesMapper.h"
#import "HippyBridge+LocalFileSource.h"
#import "HippyBridge+Private.h"
#import "HippyOCTurboModule+Inner.h"
#import "HippyTurboModuleManager.h"

#include "ios_loader.h"
#include "core/base/string_view_utils.h"
#include "core/engine.h"
#include "core/task/javascript_task.h"
#include "core/task/javascript_task_runner.h"
#include "core/napi/jsc/jsc_ctx.h"
#include "core/napi/jsc/jsc_ctx_value.h"
#include "core/scope.h"
#include "core/vm/jsc/jsc_vm.h"

NSString *const HippyJSCThreadName = @"com.tencent.hippy.JavaScript";
NSString *const HippyJavaScriptContextCreatedNotification = @"HippyJavaScriptContextCreatedNotification";
NSString *const HippyJavaScriptContextCreatedNotificationBridgeKey = @"HippyJavaScriptContextCreatedNotificationBridgeKey";

struct __attribute__((packed)) ModuleData {
    uint32_t offset;
    uint32_t size;
};

using file_ptr = std::unique_ptr<FILE, decltype(&fclose)>;
using memory_ptr = std::unique_ptr<void, decltype(&free)>;
using unicode_string_view = tdf::base::unicode_string_view;
using StringViewUtils = hippy::base::StringViewUtils;
using CallbackInfo = hippy::napi::CallbackInfo;
using FuncWrapper = hippy::napi::FuncWrapper;
using CtxValue = hippy::napi::CtxValue;


struct RandomAccessBundleData {
    file_ptr bundle;
    size_t baseOffset;
    size_t numTableEntries;
    std::unique_ptr<ModuleData[]> table;
    RandomAccessBundleData()
        : bundle(nullptr, fclose) { }
};

static bool defaultDynamicLoadAction(const unicode_string_view& uri, std::function<void(u8string)> cb, HippyBridge *bridge) {
    std::u16string u16Uri = StringViewUtils::Convert(uri, unicode_string_view::Encoding::Utf16).utf16_value();
    HippyLogInfo(@"[Hippy_OC_Log][Dynamic_Load], to default dynamic load action:%S", (const unichar*)u16Uri.c_str());
    NSString *URIString = [NSString stringWithCharacters:(const unichar*)u16Uri.c_str() length:(u16Uri.length())];
    NSURL *url = HippyURLWithString(URIString, NULL);
    if ([url isFileURL]) {
        NSString *result = [NSString stringWithContentsOfURL:url encoding:NSUTF8StringEncoding error:nil];
        u8string content(reinterpret_cast<const unicode_string_view::char8_t_*>([result UTF8String]?:""));
        cb(std::move(content));;
    }
    else {
        NSURLRequest *req = [NSURLRequest requestWithURL:url];
        [[[NSURLSession sharedSession] dataTaskWithRequest:req completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
            if (error) {
                HippyLogInfo(@"[Hippy_OC_Log][Dynamic_Load], error:%@", [error description]);
                [[bridge javaScriptExecutor] executeBlockOnJavaScriptQueue:^{
                    u8string content(reinterpret_cast<const unicode_string_view::char8_t_*>(""));
                    cb(content);
                }];
            }
            else {
                [[bridge javaScriptExecutor] executeBlockOnJavaScriptQueue:^{
                    NSString *result = @"";
                    if (data) {
                        result = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
                    }
                    u8string content(reinterpret_cast<const unicode_string_view::char8_t_*>([result UTF8String]?:""));
                    cb(std::move(content));
                }];
            }
        }] resume];
    }
    return true;
}

static bool loadFunc(const unicode_string_view& uri, std::function<void(u8string)> cb, CFTypeRef userData) {
    std::u16string u16Uri = StringViewUtils::Convert(uri, unicode_string_view::Encoding::Utf16).utf16_value();
    HippyLogInfo(@"[Hippy_OC_Log][Dynamic_Load], start load function:%S", (const unichar*)u16Uri.c_str());
    HippyBridge *strongBridge = (__bridge HippyBridge *)userData;
    if ([strongBridge.delegate respondsToSelector:@selector(dynamicLoad:URI:completion:)]) {
        NSString *URIString = [NSString stringWithCharacters:(const unichar *)u16Uri.c_str() length:u16Uri.length()];
        BOOL delegateCallRet = [strongBridge.delegate dynamicLoad:strongBridge URI:URIString completion:^(NSString *result) {
            [[strongBridge javaScriptExecutor] executeBlockOnJavaScriptQueue:^{
                u8string content(reinterpret_cast<const unicode_string_view::char8_t_*>([result UTF8String]?:""));
                cb(std::move(content));
            }];
        }];
        return delegateCallRet?:defaultDynamicLoadAction(uri, cb, strongBridge);
    }
    else {
        return defaultDynamicLoadAction(uri, cb, strongBridge);
    }
}

@implementation HippyJSCExecutor {
    // Set at setUp time:
    HippyPerformanceLogger *_performanceLogger;
    JSContext *_JSContext;
    // Set as needed:
    RandomAccessBundleData _randomAccessBundle;
    JSValueRef _batchedBridgeRef;
}

@synthesize valid = _valid;
@synthesize executorkey = _executorkey;
@synthesize bridge = _bridge;
@synthesize pScope = _pScope;
@synthesize JSGlobalContextRef = _JSGlobalContextRef;

HIPPY_EXPORT_MODULE()

- (void)setBridge:(HippyBridge *)bridge {
    _bridge = bridge;
    _performanceLogger = [bridge performanceLogger];
}

- (instancetype)initWithExecurotKey:(NSString *)execurotkey bridge:(HippyBridge *)bridge {
    if (self = [super init]) {
        _valid = YES;
        self.executorkey = execurotkey;
        self.bridge = bridge;
        std::shared_ptr<Engine> engine = [[HippyJSEnginesMapper defaultInstance] createJSEngineForKey:self.uniqueExecutorkeyForEngine];
        std::unique_ptr<Engine::RegisterMap> map = [self registerMap];
        const char *pName = [execurotkey UTF8String] ?: "";
        std::shared_ptr<Scope> scope = engine->AsyncCreateScope(pName, {}, std::move(map));
        self.pScope = scope;
        [self initURILoader];
        HippyLogInfo(@"[Hippy_OC_Log][Life_Circle],HippyJSCExecutor Init %p, execurotkey:%@", self, execurotkey);
    }

    return self;
}

- (void)initURILoader {
    std::shared_ptr<IOSLoader> loader = std::make_shared<IOSLoader>(loadFunc, (__bridge void *)_bridge);
    self.pScope->SetUriLoader(loader);
}

static unicode_string_view NSStringToU8(NSString* str) {
  std::string u8 = [str UTF8String];
  return unicode_string_view(reinterpret_cast<const unicode_string_view::char8_t_*>(u8.c_str()), u8.length());
}

- (std::unique_ptr<Engine::RegisterMap>)registerMap {
    __weak HippyJSCExecutor *weakSelf = self;
    __weak id<HippyBridgeDelegate> weakBridgeDelegate = self.bridge.delegate;
    hippy::base::RegisterFunction taskEndCB = [weakSelf](void *) {
        @autoreleasepool {
            HippyJSCExecutor *strongSelf = weakSelf;
            if (strongSelf) {
              handleJsExcepiton(strongSelf->_pScope);
              [strongSelf->_bridge handleBuffer:nil batchEnded:YES];
            }
        }
    };
    hippy::base::RegisterFunction ctxCreateCB = [weakSelf, weakBridgeDelegate](void *p) {
        @autoreleasepool {
            HippyJSCExecutor *strongSelf = weakSelf;
            if (!strongSelf) {
                return;
            }
            [strongSelf->_performanceLogger markStartForTag:HippyPLJSExecutorScopeInit];
            id<HippyBridgeDelegate> strongBridgeDelegate = weakBridgeDelegate;
            ScopeWrapper *wrapper = reinterpret_cast<ScopeWrapper *>(p);
            std::shared_ptr<Scope> scope = wrapper->scope.lock();
            if (scope) {
                std::shared_ptr<hippy::napi::JSCCtx> context = std::static_pointer_cast<hippy::napi::JSCCtx>(scope->GetContext());
                JSContext *jsContext = [strongSelf JSContext];
                auto global_object = context->GetGlobalObject();
                auto user_global_object_key = context->CreateString("global");
                context->SetProperty(global_object, user_global_object_key, global_object, hippy::napi::PropertyAttribute::DontDelete);
                NSMutableDictionary *deviceInfo = [NSMutableDictionary dictionaryWithDictionary:[strongSelf.bridge deviceInfo]];
                if ([strongBridgeDelegate respondsToSelector:@selector(objectsBeforeExecuteCode)]) {
                    NSDictionary *customObjects = [strongBridgeDelegate objectsBeforeExecuteCode];
                    if (customObjects) {
                        [deviceInfo addEntriesFromDictionary:customObjects];
                    }
                }
                if ([strongSelf.bridge isKindOfClass:[HippyBatchedBridge class]]) {
                    HippyBridge *clientBridge = [(HippyBatchedBridge *)strongSelf.bridge parentBridge];
                    NSString *clientId = [HippyDevInfo debugClientIdWithBridge:clientBridge];
                    NSDictionary *debugInfo = @{@"Debug" : @{@"debugClientId" : clientId}};
                    [deviceInfo addEntriesFromDictionary:debugInfo];
                }
                NSError *JSONSerializationError = nil;
                NSData *data = [NSJSONSerialization dataWithJSONObject:deviceInfo options:0 error:&JSONSerializationError];
                if (JSONSerializationError) {
                    NSString *errorString =
                        [NSString stringWithFormat:@"device parse error:%@, deviceInfo:%@", [JSONSerializationError localizedFailureReason], deviceInfo];
                    NSError *error = HippyErrorWithMessageAndModuleName(errorString, strongSelf.bridge.moduleName);
                    HippyFatal(error);
                }
                NSString *string = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
                context->SetProperty(global_object, context->CreateString("__HIPPYNATIVEGLOBAL__"),
                                     hippy::vm::VM::ParseJson(context, NSStringToU8(string)));
                context->SetProperty(global_object, context->CreateString("__fbBatchedBridgeConfig"),
                                     hippy::vm::VM::ParseJson(context, NSStringToU8([strongSelf.bridge moduleConfig])));
                NSString *workFolder = [strongSelf.bridge workFolder2];
                HippyAssert(workFolder, @"work folder path should not be null");
                if (workFolder) {
                    context->SetProperty(global_object, context->CreateString("__HIPPYCURDIR__"), context->CreateString(NSStringToU8(workFolder)));
                } else {
                    context->SetProperty(global_object, context->CreateString("__HIPPYCURDIR__"), context->CreateString(NSStringToU8(@"")));
                }
                installBasicSynchronousHooksOnContext(jsContext);
                jsContext[@"nativeRequireModuleConfig"] = ^NSArray *(NSString *moduleName) {
                    HippyJSCExecutor *strongSelf = weakSelf;
                    if (!strongSelf.valid) {
                        return nil;
                    }

                    NSArray *result = [strongSelf->_bridge configForModuleName:moduleName];
                    return HippyNullIfNil(result);
                };

                jsContext[@"nativeFlushQueueImmediate"] = ^(NSArray<NSArray *> *calls) {
                    HippyJSCExecutor *strongSelf = weakSelf;
                    if (!strongSelf.valid || !calls) {
                        return;
                    }
                    [strongSelf->_bridge handleBuffer:calls batchEnded:NO];
                };

                jsContext[@"nativeCallSyncHook"] = ^id(NSUInteger module, NSUInteger method, NSArray *args) {
                    HippyJSCExecutor *strongSelf = weakSelf;
                    if (!strongSelf.valid) {
                        return nil;
                    }

                    id result = [strongSelf->_bridge callNativeModule:module method:method params:args];
                    return result;
                };

    #if HIPPY_DEV
                // Inject handler used by HMR
                jsContext[@"nativeInjectHMRUpdate"] = ^(NSString *sourceCode, NSString *sourceCodeURL) {
                    HippyJSCExecutor *strongSelf = weakSelf;
                    if (!strongSelf.valid) {
                        return;
                    }
                    JSStringRef execJSString = JSStringCreateWithCFString((__bridge CFStringRef)sourceCode);
                    JSStringRef jsURL = JSStringCreateWithCFString((__bridge CFStringRef)sourceCodeURL);
                    JSEvaluateScript([strongSelf JSGlobalContextRef], execJSString, NULL, jsURL, 0, NULL);
                    JSStringRelease(jsURL);
                    JSStringRelease(execJSString);
                };
    #endif
                
                jsContext[@"getTurboModule"] = ^id (NSString *name, NSString *args) {
                    HippyJSCExecutor *strongSelf = weakSelf;
                    if (!strongSelf.valid) {
                        return nil;
                    }
                    JSValueRef value_ = [strongSelf JSTurboObjectWithName:name];
                    JSValue *objc_value = [JSValue valueWithJSValueRef:value_ inContext:[strongSelf JSContext]];
                    return objc_value;
                };
            }
            [strongSelf->_performanceLogger markStopForTag:HippyPLJSExecutorScopeInit];
        }
    };
  
    hippy::base::RegisterFunction scopeInitializedCB = [weakSelf](void *p) {
        @autoreleasepool {
            HippyJSCExecutor *strongSelf = weakSelf;
            if (!strongSelf) {
                return;
            }
            ScopeWrapper *wrapper = reinterpret_cast<ScopeWrapper *>(p);
            auto scope = wrapper->scope.lock();
            handleJsExcepiton(scope);
        }
    };
    std::unique_ptr<Engine::RegisterMap> ptr = std::make_unique<Engine::RegisterMap>();
    ptr->insert(std::make_pair(hippy::base::kAsyncTaskEndKey, taskEndCB));
    ptr->insert(std::make_pair(hippy::base::kContextCreatedCBKey, ctxCreateCB));
    ptr->insert(std::make_pair(hippy::base::KScopeInitializedCBKey, scopeInitializedCB));
    return ptr;
}





- (JSValueRef)JSTurboObjectWithName:(NSString *)name {
    //create HostObject by name
    HippyOCTurboModule *turboModule = [self->_bridge turboModuleWithName:name];
    if (!turboModule) {
        JSGlobalContextRef ctx = [self JSGlobalContextRef];
        return JSValueMakeNull(ctx);
    }

    // create jsProxy
    std::string turbo_name([name UTF8String]);
    auto scope = self->_pScope;
    if (scope->HasTurboInstance(turbo_name)) {
      auto instance = scope->GetTurboInstance(turbo_name);
      auto jsc_instance = std::static_pointer_cast<hippy::napi::JSCCtxValue>(instance);
      return jsc_instance->value_;
    }
    auto wrapper = std::make_unique<FuncWrapper>([](const CallbackInfo& info, void* data) {
      auto name = info[0];
      if (!name) {
        return;
      }
      HippyOCTurboModule *turbo = (__bridge HippyOCTurboModule*) data;
      auto turbo_wrapper = std::make_unique<TurboWrapper>(turbo, info[0]);
      auto func_wrapper = std::make_unique<FuncWrapper>([](const CallbackInfo& info, void* data) {
        std::vector<std::shared_ptr<CtxValue>> argv;
        for (size_t i = 0; i < info.Length(); ++i) {
          argv.push_back(info[i]);
        }
        auto scope_wrapper = reinterpret_cast<ScopeWrapper*>(std::any_cast<void*>(info.GetSlot()));
        auto scope = scope_wrapper->scope.lock();
        TDF_BASE_CHECK(scope);
        auto turbo_wrapper = reinterpret_cast<TurboWrapper*>(data);
        HippyOCTurboModule *turbo = turbo_wrapper->module;
        auto name = turbo_wrapper->name;
        auto result = [turbo invokeOCMethod:scope->GetContext() this_val:name args:argv.data() count:argv.size()];
        info.GetReturnValue()->Set(result);
      }, turbo_wrapper.get());
      [turbo saveTurboWrapper:name turbo:std::move(turbo_wrapper)];
      auto scope_wrapper = reinterpret_cast<ScopeWrapper*>(std::any_cast<void*>(info.GetSlot()));
      auto scope = scope_wrapper->scope.lock();
      TDF_BASE_CHECK(scope);
      auto func = scope->GetContext()->CreateFunction(func_wrapper);
      scope->SaveFuncWrapper(std::move(func_wrapper));
      info.GetReturnValue()->Set(func);
    }, (__bridge void*)turboModule);
    auto obj = scope->GetContext()->DefineProxy(wrapper);
    scope->SaveFuncWrapper(std::move(wrapper));
    scope->SetTurboInstance(turbo_name, obj);
    auto jsc_obj = std::static_pointer_cast<hippy::napi::JSCCtxValue>(obj);
    return jsc_obj->value_;
}

- (JSContext *)JSContext {
    if (nil == _JSContext) {
        JSGlobalContextRef contextRef = [self JSGlobalContextRef];
        if (contextRef) {
            _JSContext = [JSContext contextWithJSGlobalContextRef:contextRef];
#if HIPPY_DEBUG
#if defined(__IPHONE_16_4) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_16_4
            if (@available(iOS 16.4, *)) {
                [_JSContext setInspectable:YES];
            }
#endif //defined(__IPHONE_16_4) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_16_4
#endif //HIPPY_DEBUG

            HippyBridge *bridge = self.bridge;
            if ([bridge isKindOfClass:[HippyBatchedBridge class]]) {
                bridge = [(HippyBatchedBridge *)bridge parentBridge];
            }
            NSDictionary *userInfo = nil;
            if (bridge) {
                userInfo = @{ HippyJavaScriptContextCreatedNotificationBridgeKey: bridge };
            }
            if (_JSContext) {
                [[NSNotificationCenter defaultCenter] postNotificationName:HippyJavaScriptContextCreatedNotification object:nil userInfo:userInfo];
            }
        }
    }
    return _JSContext;
}

- (JSGlobalContextRef)JSGlobalContextRef {
    if (nil == _JSGlobalContextRef) {
        std::shared_ptr<Scope> scope = self.pScope;
        if (scope) {
            std::shared_ptr<hippy::napi::Ctx> napiCtx = scope->GetContext();
            std::shared_ptr<hippy::napi::JSCCtx> jscContext = std::static_pointer_cast<hippy::napi::JSCCtx>(napiCtx);
            _JSGlobalContextRef = jscContext->GetCtxRef();
        }
    }
    return _JSGlobalContextRef;
}

- (void)setUp {
}

/** Installs synchronous hooks that don't require a weak reference back to the HippyJSCExecutor. */
static void installBasicSynchronousHooksOnContext(JSContext *context) {
    context[@"nativeLoggingHook"] = ^(NSString *message, NSNumber *logLevel) {
        HippyLogLevel level = HippyLogLevelInfo;
        if (logLevel) {
            level = MAX(level, (HippyLogLevel)logLevel.integerValue);
        }

        _HippyLogJavaScriptInternal(level, message);
    };
    context[@"nativePerformanceNow"] = ^{
        return @(CACurrentMediaTime() * 1000);
    };
}

- (void)invalidate {
    if (!self.isValid) {
        return;
    }
    HippyLogInfo(@"[Hippy_OC_Log][Life_Circle],HippyJSCExecutor invalide %p", self);
    _valid = NO;
    self.pScope->WillExit();
    self.pScope = nullptr;
    _JSContext.name = @"HippyJSContext(delete)";
    _JSContext = nil;
    _JSGlobalContextRef = NULL;
    NSString *uniqueExecutorKey = self.uniqueExecutorkeyForEngine;
    dispatch_async(dispatch_get_main_queue(), ^{
        HippyLogInfo(@"[Hippy_OC_Log][Life_Circle],HippyJSCExecutor remove engine %@", uniqueExecutorKey);
        [[HippyJSEnginesMapper defaultInstance] removeEngineForKey:uniqueExecutorKey];
    });
}

- (NSString *)contextName {
    return [[self JSContext] name];
}

- (NSString *)executorkey {
    return _executorkey ?: [NSString stringWithFormat:@"%p", self];
}

- (NSString *)uniqueExecutorkeyForEngine {
    // core-engine reuse can lead to leak of context,
    // which we avoid by using a unique executor key.
    return [NSString stringWithFormat:@"%@%p", self.executorkey, self];
}


HIPPY_EXPORT_METHOD(setContextName:(NSString *)contextName) {
    __weak HippyJSCExecutor *weakSelf = self;
    [self executeBlockOnJavaScriptQueue:^{
        HippyJSCExecutor *strongSelf = weakSelf;
        if (!strongSelf) {
            return;
        }
        [[strongSelf JSContext] setName:contextName];
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            [strongSelf.bridge setUpDevClientWithName:contextName];
        });
    }];
}

- (void)dealloc {
    HippyLogInfo(@"[Hippy_OC_Log][Life_Circle],HippyJSCExecutor dealloc %p", self);
    [self invalidate];
    _randomAccessBundle.bundle.reset();
    _randomAccessBundle.table.reset();
}

- (void)secondBundleLoadCompleted:(BOOL)success {
    std::shared_ptr<Scope> scope = self.pScope;
    if (!scope) {
        return;
    }
    std::shared_ptr<hippy::napi::JSCCtx> context = std::static_pointer_cast<hippy::napi::JSCCtx>(scope->GetContext());
    HippyAssert(context != nullptr, @"secondBundleLoadCompleted get null context");
    if (nullptr == context) {
        return;
    }
    NSString *workFolder = [self.bridge workFolder2];
    HippyAssert(workFolder, @"work folder path should not be null");
    auto global_object = context->GetGlobalObject();
    if (workFolder) {
        context->SetProperty(global_object, context->CreateString("__HIPPYCURDIR__"), context->CreateString(NSStringToU8(workFolder)));
    } else {
        context->SetProperty(global_object, context->CreateString("__HIPPYCURDIR__"), context->CreateString(NSStringToU8(@"")));
    }

}

- (void)setInspectable:(BOOL)inspectable {
#if defined(__IPHONE_16_4) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_16_4
    if (@available(iOS 16.4, *)) {
        __weak HippyJSCExecutor *weakSelf = self;
        [self executeBlockOnJavaScriptQueue:^{
            HippyJSCExecutor *strongSelf = weakSelf;
            if (strongSelf) {
                [[strongSelf JSContext] setInspectable:inspectable];
            }
        }];
    }
#endif //defined(__IPHONE_16_4) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_16_4
}

- (void)updateGlobalObjectBeforeExcuteSecondary{
    if(![self.bridge.delegate respondsToSelector:@selector(objectsBeforeExecuteSecondaryCode)]){
        return;
    }
    NSDictionary *secondaryGlobal = [self.bridge.delegate objectsBeforeExecuteSecondaryCode];
    if(0 == secondaryGlobal.count){
        return;
    }
    __weak HippyJSCExecutor *weakSelf = self;
    [self executeBlockOnJavaScriptQueue:^{
        HippyJSCExecutor *strongSelf = weakSelf;
        if (!strongSelf || !strongSelf.isValid || nullptr == strongSelf.pScope) {
            return;
        }
        [strongSelf addInfoToGlobalObject:[secondaryGlobal copy]];
    }];
}

- (void)updateNativeInfoToHippyGlobalObject:(NSDictionary *)updatedInfoDict {
    if (updatedInfoDict.count <= 0){
        return;
    }
    __weak __typeof(self)weakSelf = self;
    [self executeBlockOnJavaScriptQueue:^{
        __strong __typeof(weakSelf)strongSelf = weakSelf;
        if (!strongSelf || !strongSelf.isValid || nullptr == strongSelf.pScope) {
            return;
        }
        [strongSelf addInfoToGlobalObject:updatedInfoDict.copy];
    }];
}

- (void)addInfoToGlobalObject:(NSDictionary*)addInfoDict{
    JSContext *context = [self JSContext];
    if (context) {
        JSValue *value = context[@"__HIPPYNATIVEGLOBAL__"];
        if (value) {
            for (NSString *key in addInfoDict) {
                value[key] = addInfoDict[key];
            }
        }
    }
}

- (void)flushedQueue:(HippyJavaScriptCallback)onComplete {
    // TODO: Make this function handle first class instead of dynamically dispatching it. #9317773
    [self _executeJSCall:@"flushedQueue" arguments:@[] unwrapResult:YES callback:onComplete];
}

- (void)_callFunctionOnModule:(NSString *)module
                       method:(NSString *)method
                    arguments:(NSArray *)args
                  returnValue:(BOOL)returnValue
                 unwrapResult:(BOOL)unwrapResult
                     callback:(HippyJavaScriptCallback)onComplete {
    // TODO: Make this function handle first class instead of dynamically dispatching it. #9317773
    NSString *bridgeMethod = returnValue ? @"callFunctionReturnFlushedQueue" : @"callFunctionReturnResultAndFlushedQueue";
    [self _executeJSCall:bridgeMethod arguments:@[module, method, args] unwrapResult:unwrapResult callback:onComplete];
}

- (void)callFunctionOnModule:(NSString *)moduleName
                      method:(NSString *)method
                   arguments:(NSArray *)args
                    callback:(HippyJavaScriptCallback)onComplete {
    [self _callFunctionOnModule:moduleName
                         method:method
                      arguments:args
                    returnValue:YES
                   unwrapResult:YES
                       callback:onComplete];
}

- (void)callFunctionOnModule:(NSString *)module
                      method:(NSString *)method
                   arguments:(NSArray *)args
             jsValueCallback:(HippyJavaScriptValueCallback)onComplete {
    [self _callFunctionOnModule:module method:method arguments:args returnValue:NO unwrapResult:NO callback:onComplete];
}

- (void)invokeCallbackID:(NSNumber *)cbID arguments:(NSArray *)args callback:(HippyJavaScriptCallback)onComplete {
    // TODO: Make this function handle first class instead of dynamically dispatching it. #9317773
    [self _executeJSCall:@"invokeCallbackAndReturnFlushedQueue" arguments:@[cbID, args] unwrapResult:YES callback:onComplete];
}

- (void)_executeJSCall:(NSString *)method
             arguments:(NSArray *)arguments
          unwrapResult:(BOOL)unwrapResult
              callback:(HippyJavaScriptCallback)onComplete {
    HippyAssert(onComplete != nil, @"onComplete block should not be nil");
    __weak HippyJSCExecutor *weakSelf = self;
    [self executeBlockOnJavaScriptQueue:^{
        HippyJSCExecutor *strongSelf = weakSelf;
        if (!strongSelf || !strongSelf.isValid || nullptr == strongSelf.pScope) {
            return;
        }
        @try {
            HippyBridge *bridge = [strongSelf bridge];
            NSString *moduleName = [bridge moduleName];
            NSError *executeError = nil;
            id objcValue = nil;
            std::shared_ptr<hippy::napi::Ctx> jscContext = self.pScope->GetContext();
            std::shared_ptr<hippy::napi::CtxValue> batchedbridge_value = jscContext->GetProperty(jscContext->GetGlobalObject(), "__fbBatchedBridge");
            std::shared_ptr<hippy::napi::JSCCtxValue> jsc_resultValue = nullptr;
            std::u16string exception;
            JSContext *jsContext = [strongSelf JSContext];
            JSGlobalContextRef globalContextRef = [strongSelf JSGlobalContextRef];
            if (!jsContext || !globalContextRef) {
                onComplete([NSNull null], nil);
                return;
            }
            if (batchedbridge_value) {
                std::shared_ptr<hippy::napi::CtxValue> method_value = jscContext->GetProperty(batchedbridge_value, [method UTF8String]);
                if (method_value) {
                    if (jscContext->IsFunction(method_value)) {
                        std::shared_ptr<hippy::napi::CtxValue> function_params[arguments.count];
                        for (NSUInteger i = 0; i < arguments.count; i++) {
                            JSValueRef value = [JSValue valueWithObject:arguments[i] inContext:jsContext].JSValueRef;
                            function_params[i] = std::make_shared<hippy::napi::JSCCtxValue>(globalContextRef, value);
                        }
                        hippy::napi::JSCTryCatch tryCatch(true, jscContext);
                        std::shared_ptr<hippy::napi::CtxValue> resultValue
                            = jscContext->CallFunction(method_value, arguments.count, function_params);
                        if (tryCatch.HasCaught()) {
                          exception = StringViewUtils::Convert(tryCatch.GetExceptionMsg(), unicode_string_view::Encoding::Utf16).utf16_value();
                        }
                        jsc_resultValue = std::static_pointer_cast<hippy::napi::JSCCtxValue>(resultValue);
                    } else {
                        executeError
                            = HippyErrorWithMessageAndModuleName([NSString stringWithFormat:@"%@ is not a function", method], moduleName);
                    }
                } else {
                    executeError = HippyErrorWithMessageAndModuleName(
                        [NSString stringWithFormat:@"property/function %@ not found in __fbBatchedBridge", method], moduleName);
                }
            } else {
                executeError = HippyErrorWithMessageAndModuleName(@"__fbBatchedBridge not found", moduleName);
            }
            if (!exception.empty() || executeError) {
                if (!exception.empty()) {
                    NSString *string = [NSString stringWithCharacters: reinterpret_cast<const unichar*>(exception.c_str()) length:exception.length()];
                    executeError = HippyErrorWithMessageAndModuleName(string, moduleName);
                }
            } else if (jsc_resultValue) {
                JSValueRef resutlRef = jsc_resultValue->value_;
                JSValue *objc_value = [JSValue valueWithJSValueRef:resutlRef inContext:[strongSelf JSContext]];
                objcValue = unwrapResult ? [objc_value toObject] : objc_value;
            }
            onComplete(objcValue, executeError);
        } @catch (NSException *exception) {
            NSString *moduleName = strongSelf.bridge.moduleName?:@"unknown";
            NSMutableDictionary *userInfo = [exception.userInfo mutableCopy]?:[NSMutableDictionary dictionary];
            [userInfo setObject:moduleName forKey:HippyFatalModuleName];
            [userInfo setObject:arguments?:[NSArray array] forKey:@"arguments"];
            NSException *reportException = [NSException exceptionWithName:exception.name reason:exception.reason userInfo:userInfo];
            MttHippyException(reportException);
        }
    }];
}

- (void)executeApplicationScript:(NSData *)script
                       sourceURL:(NSURL *)sourceURL
                  isCommonBundle:(BOOL)isCommonBundle
                      onComplete:(HippyJavaScriptCompleteBlock)onComplete {
    HippyAssertParam(script);
    HippyAssertParam(sourceURL);

    __weak HippyJSCExecutor *weakSelf = self;
    [self executeBlockOnJavaScriptQueue:^{
        HippyJSCExecutor *strongSelf = weakSelf;
        if (!strongSelf || !strongSelf.isValid) {
            return;
        }
        NSError *error = executeApplicationScript(script, sourceURL, isCommonBundle, strongSelf->_performanceLogger, [strongSelf JSGlobalContextRef]);
        if (onComplete) {
            onComplete(error);
        }
    }];
}

static void handleJsExcepiton(std::shared_ptr<Scope> scope) {
  if (!scope) {
    return;
  }
  std::shared_ptr<hippy::napi::JSCCtx> context = std::static_pointer_cast<hippy::napi::JSCCtx>(scope->GetContext());
  std::shared_ptr<hippy::napi::JSCCtxValue> exception = std::static_pointer_cast<hippy::napi::JSCCtxValue>(context->GetException());
  if (exception) {
    if (!context->IsExceptionHandled()) {
      context->HandleUncaughtException(exception);
    }
    std::u16string exceptionStr = StringViewUtils::Convert(context->GetExceptionMsg(exception), unicode_string_view::Encoding::Utf16).utf16_value();
    NSString *err = [NSString stringWithCharacters:(const unichar *)exceptionStr.c_str() length:(exceptionStr.length())];
    NSError *error = HippyErrorWithMessage(err);
    // NSError *error = RCTErrorWithMessageAndModule(err, strongSelf.bridge.moduleName);
    HippyFatal(error);
    context->SetException(nullptr);
    context->SetExceptionHandled(true);
  }
}

static NSLock *jslock() {
    static dispatch_once_t onceToken;
    static NSLock *lock = nil;
    dispatch_once(&onceToken, ^{
        lock = [[NSLock alloc] init];
    });
    return lock;
}

static NSError *executeApplicationScript(NSData *script,
                                         NSURL *sourceURL,
                                         BOOL isCommonBundle,
                                         HippyPerformanceLogger *performanceLogger,
                                         JSGlobalContextRef ctx) {
    @autoreleasepool {
        NSString *absoluteSourceUrl = [sourceURL absoluteString];
        HippyLogInfo(@"load script begin, length %zd for url %@", [script length], absoluteSourceUrl);
        if (isCommonBundle) {
            [performanceLogger markStartForTag:HippyPLCommonScriptExecution];
        } else {
            [performanceLogger markStartForTag:HippyPLSecondaryScriptExecution];
        }
        
        JSValueRef jsError = NULL;
        NSString *scriptText = [[NSString alloc] initWithData:script encoding:NSUTF8StringEncoding];
        if (!absoluteSourceUrl) {
            HippyLogError(@"Missing bundle URL for script execution");
            return [NSError errorWithDomain:HippyErrorDomain
                                       code:NSFileNoSuchFileError
                                   userInfo:@{
                NSLocalizedDescriptionKey: @"JavaScript bundle URL is required",
            }];
        }
        
        if (!scriptText) {
            HippyLogError(@"Script decoding failed, data length: %lu bytes", (unsigned long)script.length);
            return [NSError errorWithDomain:HippyErrorDomain
                                       code:NSFileReadInapplicableStringEncodingError
                                   userInfo:@{
                NSLocalizedDescriptionKey: @"Failed to decode script content",
            }];
        }
        JSStringRef execJSString = JSStringCreateWithCFString((__bridge CFStringRef)scriptText);
        JSStringRef bundleURL = JSStringCreateWithCFString((__bridge CFStringRef)absoluteSourceUrl);


        NSLock *lock = jslock();
        BOOL lockSuccess = [lock lockBeforeDate:[NSDate dateWithTimeIntervalSinceNow:1]];
        JSEvaluateScript(ctx, execJSString, NULL, bundleURL, 0, &jsError);
        JSStringRelease(bundleURL);
        JSStringRelease(execJSString);
        if (lockSuccess) {
            [lock unlock];
        }
        if (isCommonBundle) {
            [performanceLogger markStopForTag:HippyPLCommonScriptExecution];
        } else {
            [performanceLogger markStopForTag:HippyPLSecondaryScriptExecution];
        }

        NSError *error = jsError ? HippyNSErrorFromJSErrorRef(jsError, ctx) : nil;
        // HIPPY_PROFILE_END_EVENT(0, @"js_call");
        HippyLogInfo(@"load script end,length %zd for url %@, error %@", [script length], absoluteSourceUrl, [error description]);

        return error;
    }
}

- (void)executeBlockOnJavaScriptQueue:(dispatch_block_t)block {
    auto engine = [[HippyJSEnginesMapper defaultInstance] JSEngineForKey:self.uniqueExecutorkeyForEngine];
    if (engine) {
        dispatch_block_t autoReleaseBlock = ^(void){
            if (block) {
                @autoreleasepool {
                    block();
                }
            }
        };
        if (engine->GetJSRunner()->IsJsThread() == false) {
            std::shared_ptr<JavaScriptTask> task = std::make_shared<JavaScriptTask>();
            task->callback = autoReleaseBlock;
            engine->GetJSRunner()->PostTask(task);
        } else {
            autoReleaseBlock();
        }
    }
}

- (void)executeAsyncBlockOnJavaScriptQueue:(dispatch_block_t)block {
    auto engine = [[HippyJSEnginesMapper defaultInstance] JSEngineForKey:self.uniqueExecutorkeyForEngine];
    if (engine) {
        std::shared_ptr<JavaScriptTask> task = std::make_shared<JavaScriptTask>();
        dispatch_block_t autoReleaseBlock = ^(void){
            if (block) {
                @autoreleasepool {
                    block();
                }
            }
        };
        task->callback = autoReleaseBlock;
        engine->GetJSRunner()->PostTask(task);
    }
}

- (void)injectJSONText:(NSString *)script asGlobalObjectNamed:(NSString *)objectName callback:(HippyJavaScriptCompleteBlock)onComplete {
    NSAssert(nil != script, @"param 'script' can't be nil");
    if (nil == script) {
        if (onComplete) {
            NSString *errorMessage = [NSString stringWithFormat:@"param 'script' is nil"];
            NSError *error = [NSError errorWithDomain:HippyErrorDomain code:2 userInfo:@{ NSLocalizedDescriptionKey: errorMessage }];
            onComplete(error);
        }
        return;
    }
    if (HIPPY_DEBUG) {
        HippyAssert(HippyJSONParse(script, NULL) != nil, @"%@ wasn't valid JSON!", script);
    }

    __weak HippyJSCExecutor *weakSelf = self;
    [self executeBlockOnJavaScriptQueue:^{
        HippyJSCExecutor *strongSelf = weakSelf;
        if (!strongSelf || !strongSelf.isValid) {
            return;
        }

        // HIPPY_PROFILE_BEGIN_EVENT(0, @"injectJSONText", @{@"objectName": objectName});
        JSStringRef execJSString = JSStringCreateWithCFString((__bridge CFStringRef)script);
        JSGlobalContextRef ctx = [strongSelf JSGlobalContextRef];
        JSValueRef valueToInject = JSValueMakeFromJSONString(ctx, execJSString);
        JSStringRelease(execJSString);

        NSError *error;
        if (!valueToInject) {
            NSString *errorMessage = [NSString stringWithFormat:@"Can't make JSON value from script '%@'", script];
            error = [NSError errorWithDomain:HippyErrorDomain code:2 userInfo:@ { NSLocalizedDescriptionKey: errorMessage }];
            HippyLogError(@"%@", errorMessage);
        } else {
            JSObjectRef globalObject = JSContextGetGlobalObject(ctx);
            JSStringRef JSName = JSStringCreateWithCFString((__bridge CFStringRef)objectName);
            JSValueRef jsError = NULL;
            JSObjectSetProperty(ctx, globalObject, JSName, valueToInject, kJSPropertyAttributeNone, &jsError);
            JSStringRelease(JSName);

            if (jsError) {
                error = HippyNSErrorFromJSErrorRef(jsError, ctx);
            }
        }
        // HIPPY_PROFILE_END_EVENT(0, @"js_call,json_call");

        if (onComplete) {
            onComplete(error);
        }
    }];
}

static bool readRandomAccessModule(const RandomAccessBundleData &bundleData, size_t offset, size_t size, char *data) {
    return fseek(bundleData.bundle.get(), offset + bundleData.baseOffset, SEEK_SET) == 0 && fread(data, 1, size, bundleData.bundle.get()) == size;
}

static void executeRandomAccessModule(HippyJSCExecutor *executor, uint32_t moduleID, size_t offset, size_t size) {
    auto data = std::make_unique<char[]>(size);
    if (!readRandomAccessModule(executor->_randomAccessBundle, offset, size, data.get())) {
        HippyFatal(HippyErrorWithMessage(@"Error loading RAM module"));
        return;
    }

    char url[14];  // 10 = maximum decimal digits in a 32bit unsigned int + ".js" + null byte
    snprintf(url, sizeof(url), "%" PRIu32 ".js", moduleID);

    JSStringRef code = JSStringCreateWithUTF8CString(data.get());
    JSValueRef jsError = NULL;
    JSStringRef sourceURL = JSStringCreateWithUTF8CString(url);
    JSGlobalContextRef ctx = [executor JSGlobalContextRef];
    JSValueRef result = JSEvaluateScript(ctx, code, NULL, sourceURL, 0, &jsError);

    JSStringRelease(code);
    JSStringRelease(sourceURL);

    if (!result) {
        NSError *error = HippyNSErrorFromJSErrorRef(jsError, ctx);
        dispatch_async(dispatch_get_main_queue(), ^{
            HippyFatal(error);
            [executor invalidate];
        });
    }
}

- (void)_nativeRequire:(NSNumber *)moduleID {
    if (!moduleID) {
        return;
    }

    const uint32_t ID = [moduleID unsignedIntValue];

    if (ID < _randomAccessBundle.numTableEntries) {
        ModuleData *moduleData = &_randomAccessBundle.table[ID];
        const uint32_t size = NSSwapLittleIntToHost(moduleData->size);

        // sparse entry in the table -- module does not exist or is contained in the startup section
        if (size == 0) {
            return;
        }

        executeRandomAccessModule(self, ID, NSSwapLittleIntToHost(moduleData->offset), size);
    }
}

@end
