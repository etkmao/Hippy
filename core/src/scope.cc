/*
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
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "core/scope.h"

#include <future>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/logging.h"
#include "core/modules/console_module.h"
#include "core/modules/timer_module.h"
#include "core/modules/contextify_module.h"
#include "core/task/javascript_task.h"
#include "core/task/javascript_task_runner.h"
#include "core/vm/native_source_code.h"
#ifdef JS_V8
#include "core/napi/v8/v8_ctx.h"
#include "core/vm/v8/memory_module.h"
#endif

// TODO:added
#include "core/modules/module_register.h"
#include "core/vm/native_source_code.h"
#include "core/napi/jsc/jsc_ctx.h"
#include "core/napi/jsc/jsc_ctx_value.h"
#include "core/napi/callback_info.h"

using unicode_string_view = tdf::base::unicode_string_view;

using RegisterMap = hippy::base::RegisterMap;
using RegisterFunction = hippy::base::RegisterFunction;
using CtxValue = hippy::napi::CtxValue;
using ModuleClassMap = hippy::napi::ModuleClassMap;
using BindingData = hippy::napi::BindingData;
using JSCCtx = hippy::napi::JSCCtx;
using JSCCtxValue = hippy::napi::JSCCtxValue;
using ModuleClass = hippy::napi::ModuleClass;
using FunctionData = hippy::napi::FunctionData;
using JsCallback = hippy::napi::JsCallback;
using CallbackInfo = hippy::napi::CallbackInfo;

constexpr char kDeallocFuncName[] = "HippyDealloc";
constexpr char kHippyBootstrapJSName[] = "bootstrap.js";

std::unique_ptr<BindingData> binding_data_;

JSObjectRef RegisterModule2(std::shared_ptr<Scope> scope,
                           JSContextRef ctx,
                           const unicode_string_view& module_name,
                            ModuleClass module);

static void InternalBindingCallback(const hippy::napi::CallbackInfo& info, void* data) {
//  auto scope_wrapper = reinterpret_cast<ScopeWrapper*>(0);
  auto scope = info.GetScope(); //scope_wrapper->scope.lock();
  TDF_BASE_CHECK(scope);
  auto context = scope->GetContext();
  unicode_string_view module_name;
  context->GetValueString(info[0], &module_name);
  auto u8_module_name = hippy::base::StringViewUtils::ToU8StdStr(module_name);
  auto module_object = scope->GetModuleObject(u8_module_name);
  // TDF_BASE_CHECK(module_object);
  // todo(polly) MemoryModule
  if (!module_object) {
    return;
  }
  auto len = info.Length();
  auto argc = len > 1 ? (len - 1) : 0;
  std::shared_ptr<CtxValue> rest_args[argc];
  for (size_t i = 0; i < argc; ++i) {
    rest_args[i] = info[i + 1];
  }
  
  // TODO:changed
//  module_name = unicode_string_view(u8_module_name);  // content is latin
//  ModuleClassMap module_class_map = binding_data_->map_;
//  auto it = module_class_map.find(module_name);
//  if (it == module_class_map.end()) {
//    return;
//  }
//  auto jscctx = std::static_pointer_cast<JSCCtx>(context);
//  auto js_object = RegisterModule2(scope, jscctx->context_, module_name, it->second);

//  info.GetReturnValue()->Set(std::make_shared<JSCCtxValue>(jscctx->context_, js_object));
  
  
  auto js_object = module_object->BindFunction(scope, rest_args);
  info.GetReturnValue()->Set(js_object);

}

Scope::Scope(std::weak_ptr<Engine> engine,
             std::string name,
             std::unique_ptr<RegisterMap> map)
    : engine_(std::move(engine)), context_(nullptr), name_(std::move(name)), map_(std::move(map)) {
}

Scope::~Scope() {
  TDF_BASE_DLOG(INFO) << "~Scope";
}

void Scope::WillExit() {
  TDF_BASE_DLOG(INFO) << "WillExit begin";
  std::promise<std::shared_ptr<CtxValue>> promise;
  std::future<std::shared_ptr<CtxValue>> future = promise.get_future();
  std::weak_ptr<Ctx> weak_context = context_;
  JavaScriptTask::Function cb = hippy::base::MakeCopyable(
      [weak_context, p = std::move(promise)]() mutable {
        TDF_BASE_LOG(INFO) << "run js WillExit begin";
        std::shared_ptr<CtxValue> rst = nullptr;
        auto context = weak_context.lock();
        if (context) {
          auto global_object = context->GetGlobalObject();
          auto func_name = context->CreateString(kDeallocFuncName);
          auto fn = context->GetProperty(global_object, func_name);
          bool is_fn = context->IsFunction(fn);
          if (is_fn) {
            context->CallFunction(fn, 0, nullptr);
          }
        }
        p.set_value(rst);
      });
  auto runner = GetTaskRunner();
  if (runner->IsJsThread()) {
    cb();
  } else {
    std::shared_ptr<JavaScriptTask> task = std::make_shared<JavaScriptTask>();
    task->callback = cb;
    runner->PostTask(task);
  }

  future.get();
  TDF_BASE_DLOG(INFO) << "ExitCtx end";
}

void Scope::Init() {
  CreateContext();
  BindModule();
  Bootstrap();
  InvokeCallback();
}

void Scope::CreateContext() {
  auto engine = engine_.lock();
  TDF_BASE_CHECK(engine);
  context_ = engine->GetVM()->CreateContext();
  TDF_BASE_CHECK(context_);
  context_->SetExternalData(GetScopeWrapperPointer());
  if (map_) {
    auto it = map_->find(hippy::base::kContextCreatedCBKey);
    if (it != map_->end()) {
      auto f = it->second;
      if (f) {
        f(wrapper_.get());
        map_->erase(it);
      }
    }
  }
}


void Scope::BindModule() {
  module_object_map_["ConsoleModule"] = std::make_shared<ConsoleModule>();
  module_object_map_["TimerModule"] = std::make_shared<TimerModule>();
  module_object_map_["ContextifyModule"] = std::make_shared<ContextifyModule>();
#ifdef JS_V8
  module_object_map_["MemoryModule"] = std::make_shared<MemoryModule>();
#endif
}

void Scope::Bootstrap() {
  TDF_BASE_LOG(INFO) << "Bootstrap begin";
  auto source_code = hippy::GetNativeSourceCode(kHippyBootstrapJSName);
  TDF_BASE_DCHECK(source_code.data_ && source_code.length_);
  unicode_string_view str_view(source_code.data_, source_code.length_);
  auto function = context_->RunScript(str_view, kHippyBootstrapJSName);
  auto is_func = context_->IsFunction(function);
  TDF_BASE_CHECK(is_func) << "bootstrap return not function, len = " << source_code.length_;
  auto func_wrapper = std::make_shared<hippy::napi::FuncWrapper>(InternalBindingCallback, nullptr);
  std::shared_ptr<CtxValue> argv[] = { context_->CreateFunction(func_wrapper) };
  SaveFuncWrapper(func_wrapper);
  context_->CallFunction(function, 1, argv);
}

void Scope::InvokeCallback() {
  if (!map_) {
    return;
  }
  auto it = map_->find(hippy::base::KScopeInitializedCBKey);
  if (it != map_->end()) {
    auto f = it->second;
    if (f) {
      f(wrapper_.get());
      map_->erase(it);
    }
  }
}

void* Scope::GetScopeWrapperPointer() {
  TDF_BASE_CHECK(wrapper_);
  return wrapper_.get();
}

void Scope::RunJS(const unicode_string_view& data,
                  const unicode_string_view& name,
                  bool is_copy) {
  std::weak_ptr<Ctx> weak_context = context_;
  JavaScriptTask::Function callback = [data, name, is_copy, weak_context] {
#ifdef JS_V8
    auto context = std::static_pointer_cast<hippy::napi::V8Ctx>(weak_context.lock());
    if (context) {
      context->RunScript(data, name, false, nullptr, is_copy);
    }
#else
    auto context = weak_context.lock();
    if (context) {
      context->RunScript(data, name);
    }
#endif
  };

  auto runner = GetTaskRunner();
  if (runner->IsJsThread()) {
    callback();
  } else {
    std::shared_ptr<JavaScriptTask> task = std::make_shared<JavaScriptTask>();
    task->callback = callback;
    runner->PostTask(task);
  }
}

std::shared_ptr<CtxValue> Scope::RunJSSync(const unicode_string_view& data,
                                           const unicode_string_view& name,
                                           bool is_copy) {
  std::promise<std::shared_ptr<CtxValue>> promise;
  std::future<std::shared_ptr<CtxValue>> future = promise.get_future();
  std::weak_ptr<Ctx> weak_context = context_;
  JavaScriptTask::Function cb = hippy::base::MakeCopyable(
      [data, name, is_copy, weak_context, p = std::move(promise)]() mutable {
        std::shared_ptr<CtxValue> rst = nullptr;
#ifdef JS_V8
        auto context = std::static_pointer_cast<hippy::napi::V8Ctx>(weak_context.lock());
        if (context) {
          rst = context->RunScript(data, name, false, nullptr, is_copy);
        }
#else
        auto context = weak_context.lock();
        if (context) {
          rst = context->RunScript(data, name);
        }
#endif
        p.set_value(rst);
      });

  auto runner = GetTaskRunner();
  if (runner->IsJsThread()) {
    cb();
  } else {
    std::shared_ptr<JavaScriptTask> task = std::make_shared<JavaScriptTask>();
    task->callback = cb;
    runner->PostTask(task);
  }
  std::shared_ptr<CtxValue> ret = future.get();
  return ret;
}


// TODO:added

ModuleBase* Scope::GetModuleClass(const unicode_string_view& moduleName) {
  auto it = module_class_map_.find(moduleName);
  return it != module_class_map_.end() ? it->second.get() : nullptr;
}

void Scope::AddModuleClass(const unicode_string_view& name,
                           std::unique_ptr<ModuleBase> module) {
  module_class_map_.insert({name, std::move(module)});
}

std::shared_ptr<hippy::napi::CtxValue> Scope::GetModuleValue(
    const unicode_string_view& moduleName) {
  auto it = module_value_map_.find(moduleName);
  return it != module_value_map_.end() ? it->second : nullptr;
}

void Scope::AddModuleValue(const unicode_string_view& name,
                           const std::shared_ptr<CtxValue>& value) {
  module_value_map_.insert({name, value});
}

void Scope::SaveFunctionData(std::unique_ptr<hippy::napi::FunctionData> data) {
  function_data_.push_back(std::move(data));
}

JSValueRef JsCallbackFunc2(JSContextRef ctx,
                          JSObjectRef function,
                          JSObjectRef thisObject,
                          size_t argumentCount,
                          const JSValueRef arguments[],
                          JSValueRef* exception_ref) {
  void* data = JSObjectGetPrivate(function);
  if (!data) {
    return JSValueMakeUndefined(ctx);
  }
  FunctionData* fn_data = reinterpret_cast<FunctionData*>(data);
  std::shared_ptr<Scope> scope = fn_data->scope_.lock();
  if (!scope) {
    return JSValueMakeUndefined(ctx);
  }
  JsCallback cb = fn_data->callback_;
  std::shared_ptr<JSCCtx> context =
      std::static_pointer_cast<JSCCtx>(scope->GetContext());
  CallbackInfo info{scope};

  
  for (size_t i = 0; i < argumentCount; i++) {
    info.AddValue(
        std::make_shared<JSCCtxValue>(context->GetCtxRef(), arguments[i]));
  }
  cb(info, 0);

  std::shared_ptr<JSCCtxValue> exception =
      std::static_pointer_cast<JSCCtxValue>(info.GetExceptionValue()->Get());
  if (exception) {
    *exception_ref = exception->value_;
    return JSValueMakeUndefined(ctx);
  }

  std::shared_ptr<JSCCtxValue> ret_value =
      std::static_pointer_cast<JSCCtxValue>(info.GetReturnValue()->Get());
  if (!ret_value) {
    return JSValueMakeUndefined(ctx);
  }

  JSValueRef valueRef = ret_value->value_;
  return valueRef;
}

JSObjectRef RegisterModule2(std::shared_ptr<Scope> scope,
                           JSContextRef ctx,
                           const unicode_string_view& module_name,
                           ModuleClass module) {
  JSClassDefinition cls_def = kJSClassDefinitionEmpty;
  TDF_BASE_DCHECK(module_name.encoding() ==
                  unicode_string_view::Encoding::Latin1);
  cls_def.className = module_name.latin1_value().c_str();
  JSClassRef cls_ref = JSClassCreate(&cls_def);
  JSObjectRef module_obj = JSObjectMake(ctx, cls_ref, nullptr);
  JSClassRelease(cls_ref);
  for (auto fn : module) {
    JSClassDefinition fn_def = kJSClassDefinitionEmpty;
    TDF_BASE_DCHECK(fn.first.encoding() ==
                    unicode_string_view::Encoding::Latin1);
    fn_def.className = fn.first.latin1_value().c_str();
    fn_def.callAsFunction = JsCallbackFunc2;
    std::unique_ptr<FunctionData> fn_data =
        std::make_unique<FunctionData>(scope, fn.second);
    JSClassRef fn_ref = JSClassCreate(&fn_def);
    JSObjectRef fn_obj =
        JSObjectMake(ctx, fn_ref, reinterpret_cast<void*>(fn_data.get()));
    JSStringRef fn_str_ref = JSStringCreateWithUTF8CString(fn_def.className);
    JSObjectSetProperty(ctx, module_obj, fn_str_ref, fn_obj,
                        kJSPropertyAttributeReadOnly, nullptr);
    JSStringRelease(fn_str_ref);
    JSClassRelease(fn_ref);
    scope->SaveFunctionData(std::move(fn_data));
  }

  std::shared_ptr<JSCCtx> context =
      std::static_pointer_cast<JSCCtx>(scope->GetContext());
  std::shared_ptr<JSCCtxValue> module_value =
      std::make_shared<JSCCtxValue>(context->GetCtxRef(), module_obj);
  scope->AddModuleValue(module_name, module_value);
  return module_obj;
}

JSValueRef GetInternalBinding(JSContextRef ctx,
                              JSObjectRef function,
                              JSObjectRef thisObject,
                              size_t argc,
                              const JSValueRef argv[],
                              JSValueRef* exception) {
  if (argc <= 0) {
    return JSValueMakeNull(ctx);
  }

  JSValueRef name_ref = argv[0];
  if (!JSValueIsString(ctx, name_ref)) {
    return JSValueMakeNull(ctx);
  }

  BindingData* binding_data =
      reinterpret_cast<BindingData*>(JSObjectGetPrivate(function));
  std::shared_ptr<Scope> scope = binding_data->scope_.lock();
  if (!scope) {
    return JSValueMakeNull(ctx);
  }

  JSStringRef name_str_ref = JSValueToStringCopy(ctx, name_ref, nullptr);
  unicode_string_view module_name = hippy::napi::ToStrView(name_str_ref);
  JSStringRelease(name_str_ref);

  std::string module_name_str = hippy::base::StringViewUtils::ToU8StdStr(module_name);
  module_name = unicode_string_view(module_name_str);  // content is latin

  std::shared_ptr<JSCCtxValue> module_value =
      std::static_pointer_cast<JSCCtxValue>(scope->GetModuleValue(module_name));
  if (module_value) {
    return module_value->value_;
  }

  ModuleClassMap module_class_map = binding_data->map_;
  auto it = module_class_map.find(module_name);
  if (it == module_class_map.end()) {
    return JSValueMakeNull(ctx);
  }

  return RegisterModule2(scope, ctx, module_name, it->second);
}

std::shared_ptr<CtxValue> GetInternalBindingFn(const std::shared_ptr<Scope>& scope) {
  std::shared_ptr<JSCCtx> context =
      std::static_pointer_cast<JSCCtx>(scope->GetContext());
  JSClassDefinition cls_def = kJSClassDefinitionEmpty;
  cls_def.callAsFunction = GetInternalBinding;
  JSClassRef cls_ref = JSClassCreate(&cls_def);
  JSObjectRef func = JSObjectMake(context->GetCtxRef(), cls_ref,
                                  scope->GetBindingData().get());
  JSClassRelease(cls_ref);
  return std::static_pointer_cast<CtxValue>(std::make_shared<JSCCtxValue>(context->GetCtxRef(), func));
}

void Scope::Initialized() {
  TDF_BASE_DLOG(INFO) << "Scope Initialized";
  CreateContext();
//  context_ = engine_.lock()->GetVM()->CreateContext();
//  if (context_ == nullptr) {
//    TDF_BASE_DLOG(ERROR) << "CreateContext return nullptr";
//    return;
//  }
  std::shared_ptr<Scope> self = wrapper_->scope.lock();
  if (!self) {
    TDF_BASE_DLOG(ERROR) << "Scope wrapper_ error_";
    return;
  }
//  RegisterMap::const_iterator it =
//      map_->find(hippy::base::kContextCreatedCBKey);
//  if (it != map_->end()) {
//    RegisterFunction f = it->second;
//    if (f) {
//      TDF_BASE_DLOG(INFO) << "run ContextCreatedCB begin";
//      f(wrapper_.get());
//      TDF_BASE_DLOG(INFO) << "run ContextCreatedCB end";
//      map_->erase(it);
//    }
//  }
  
  BindModule();
  
  
  ModuleClassMap map(ModuleRegister::instance()->GetInternalList());
  binding_data_ = std::make_unique<BindingData>(self, map);

  
  Bootstrap();
  
//  auto source_code = hippy::GetNativeSourceCode(kHippyBootstrapJSName);
//  TDF_BASE_DCHECK(source_code.data_ && source_code.length_);
//  unicode_string_view str_view(reinterpret_cast<const unicode_string_view::char8_t_ *>(source_code.data_),
//                               source_code.length_);
//  std::shared_ptr<CtxValue> function = context_->RunScript(
//      str_view, kHippyBootstrapJSName);
//
//  bool is_func = context_->IsFunction(function);
//  TDF_BASE_CHECK(is_func) << "bootstrap return not function, len = " << source_code.length_;
//  // TODO(super): The following statement will be removed when TDF_BASE_CHECK will be cause abort
//  if (!is_func) {
//    return;
//  }
//
//  std::shared_ptr<CtxValue> internal_binding_fn =
//      GetInternalBindingFn(self);
//  std::shared_ptr<CtxValue> argv[] = {internal_binding_fn};
//  context_->CallFunction(function, 1, argv);

  InvokeCallback();
//  auto it = map_->find(hippy::base::KScopeInitializedCBKey);
//  if (it != map_->end()) {
//    RegisterFunction f = it->second;
//    if (f) {
//      TDF_BASE_DLOG(INFO) << "run SCOPE_INITIALIZED begin";
//      f(wrapper_.get());
//      TDF_BASE_DLOG(INFO) << "run SCOPE_INITIALIZED end";
//      map_->erase(it);
//    }
//  }
}
