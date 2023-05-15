#include "driver/module_dispatcher.h"

#include <utility>

#include "config.h"
#include "driver/js_driver_utils.h"
#include "driver/napi/v8/v8_ctx.h"
#include "driver/scope.h"
#include "driver/vm/v8/v8_vm.h"
#include "footstone/deserializer.h"
#include "footstone/logging.h"
#include "footstone/string_utils.h"
#include "footstone/string_view.h"
#include "footstone/string_view_utils.h"
#include "vfs/uri_loader.h"

namespace hippy {
inline namespace windows {
inline namespace framework {

constexpr char kStorageModule[] = "StorageModule";
constexpr char kStroageFunctionGetItemsValue[] = "multiGet";
constexpr char kStroageFunctionSetItemsValue[] = "multiSet";
constexpr char kStroageFunctionRemoveItems[] = "multiRemove";
constexpr char kStroageFunctionGetAllItemsKey[] = "getAllKeys";
constexpr char kWebsocketModule[] = "websocket";
constexpr char kWebsocketFunctionConnect[] = "connect";
constexpr char kWebsocketFunctionClose[] = "close";
constexpr char kWebsocketFunctionSend[] = "send";
constexpr char kNetInfoModule[] = "NetInfo";
constexpr char kNetInfoFunctionGetCurrentConnectivity[] = "getCurrentConnectivity";
constexpr char kNetworkModule[] = "network";
constexpr char kNetworkFunctionFetch[] = "fetch";
constexpr char kNetworkFunctionGetCookie[] = "getCookie";
constexpr char kNetworkFunctionSetCookie[] = "setCookie";
constexpr char kClipboardModule[] = "ClipboardModule";
constexpr char kClipboardFunctionGetString[] = "getString";
constexpr char kClipboardFunctionSetString[] = "setString";
constexpr char kImageLoaderModule[] = "ImageLoaderModule";
constexpr char kImageLoaderFunctionGetSize[] = "getSize";
constexpr char kImageLoaderFunctionPrefetch[] = "prefetch";

constexpr uint32_t kSuccess = 0;
constexpr uint32_t kError = 2;

ModuleDispatcher::ModuleDispatcher()
    : clipboard_module_(std::make_shared<hippy::windows::framework::module::Clipboard>()),
      image_loader_module_(std::make_shared<hippy::windows::framework::module::ImageLoader>()),
      net_info_module_(std::make_shared<hippy::windows::framework::module::NetInfo>()),
      network_module_(std::make_shared<hippy::windows::framework::module::Network>()),
      websocket_module_(std::make_shared<hippy::windows::framework::module::Websocket>()),
      storage_module_(std::make_shared<hippy::windows::framework::module::Storage>()){};

void ModuleDispatcher::Initial(const std::shared_ptr<hippy::Config>& config) {
  clipboard_module_->Initial();
  image_loader_module_->Initial();
  net_info_module_->Initial();
  network_module_->Initial();
  websocket_module_->Initial();
  storage_module_->Initial();
};

void ModuleDispatcher::Dispatcher(const CallbackInfo& info, const std::shared_ptr<Scope>& scope) {
  FOOTSTONE_DLOG(INFO) << "Call Windows Module";
  auto context = scope->GetContext();

  string_view module_name;
  if (info[0]) {
    if (!context->GetValueString(info[0], &module_name)) {
      info.GetExceptionValue()->Set(context, "module name error");
      return;
    }
    FOOTSTONE_DLOG(INFO) << "CallJava module_name = " << module_name;
  } else {
    info.GetExceptionValue()->Set(context, "info error");
    return;
  }

  string_view fn_name;
  if (info[1]) {
    if (!context->GetValueString(info[1], &fn_name)) {
      info.GetExceptionValue()->Set(context, "func name error");
      return;
    }
    FOOTSTONE_DLOG(INFO) << "CallJava fn_name = " << fn_name;
  } else {
    info.GetExceptionValue()->Set(context, "info error");
    return;
  }

  string_view cb_id_str;
  if (info[2]) {
    double cb_id;
    if (context->GetValueString(info[2], &cb_id_str)) {
      FOOTSTONE_DLOG(INFO) << "CallJava cb_id = " << cb_id_str;
    } else if (context->GetValueNumber(info[2], &cb_id)) {
      cb_id_str = std::to_string(cb_id);
      FOOTSTONE_DLOG(INFO) << "CallJava cb_id = " << cb_id_str;
    }
  }

  std::string buffer_data;
  footstone::value::HippyValue value;
  if (info[3] && context->IsObject(info[3])) {
    auto engine = scope->GetEngine().lock();
    FOOTSTONE_DCHECK(engine);
    if (!engine) {
      return;
    }
    auto vm = engine->GetVM();
    auto v8_vm = std::static_pointer_cast<hippy::driver::vm::V8VM>(vm);
    if (v8_vm->IsEnableV8Serialization()) {
      auto v8_ctx = std::static_pointer_cast<hippy::napi::V8Ctx>(context);
      buffer_data = v8_ctx->GetSerializationBuffer(info[3], v8_vm->GetBuffer());
      footstone::Deserializer deserializer(reinterpret_cast<const uint8_t*>(buffer_data.c_str()), buffer_data.length());
      deserializer.ReadHeader();
      deserializer.ReadValue(value);
    }
  }

  int32_t transfer_type = 0;
  if (info[4]) {
    context->GetValueNumber(info[4], &transfer_type);
  }

  FOOTSTONE_DLOG(INFO) << "module name = " << module_name << ", function name = " << fn_name
                       << ", parameter = " << buffer_data;
  if (module_name == kClipboardModule) {
    ClipboardModuleHandle(scope, fn_name, cb_id_str, value);
  } else if (module_name == kImageLoaderModule) {
    ImageLoaderModuleHandle(scope, fn_name, cb_id_str, value);
  } else if (module_name == kNetInfoModule) {
    NetInfoModuleHandle(scope, fn_name, cb_id_str, value);
  } else if (module_name == kNetworkModule) {
    NetworkModuleHandle(scope, fn_name, cb_id_str, value);
  } else if (module_name == kWebsocketModule) {
    WebsocketModuleHandle(scope, fn_name, cb_id_str, value);
  } else if (module_name == kStorageModule) {
    StorageModuleHandle(scope, fn_name, cb_id_str, value);
  } else {
    FOOTSTONE_LOG(WARNING) << "module " << module_name << " is not support !!!";
  }
}

void ModuleDispatcher::CallJs(const std::shared_ptr<Scope>& scope, const std::string& module_name,
                              const std::string& function_name, const string_view& callback_id,
                              const footstone::value::HippyValue& result,
                              const footstone::value::HippyValue& callback_parameters) {
  serializer_.Release();
  serializer_.WriteHeader();
  footstone::HippyValue::HippyValueObjectType object;
  object.insert({"result", result});
  object.insert({"moduleName", footstone::HippyValue(module_name)});
  object.insert({"moduleFunc", footstone::HippyValue(function_name)});
  object.insert({"callId", footstone::HippyValue(callback_id.latin1_value())});
  object.insert({"params", callback_parameters});
  serializer_.WriteValue(footstone::HippyValue(object));
  std::pair<uint8_t*, size_t> buffer = serializer_.Release();
  byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
  auto call_js_callback = [](hippy::driver::CALL_FUNCTION_CB_STATE state, const string_view& msg) {
    FOOTSTONE_DLOG(INFO) << msg;
  };
  auto on_js_runner = []() {};
  JsDriverUtils::CallJs(u"callBack", scope, call_js_callback, buffer_data, on_js_runner);
}

void ModuleDispatcher::StorageModuleHandle(const std::shared_ptr<Scope>& scope, const string_view& func,
                                           const string_view& cb_id, const footstone::value::HippyValue& buffer) {
  if (storage_module_ == nullptr) return;
  if (func == kStroageFunctionGetItemsValue) {
    auto success_callback = [WEAK_THIS, scope, cb_id](const footstone::value::HippyValue& params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kStorageModule, kStroageFunctionGetItemsValue, cb_id, footstone::value::HippyValue(kSuccess),
                   params);
    };
    auto fail_callback = [WEAK_THIS, scope, cb_id](const footstone::value::HippyValue& params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kStorageModule, kStroageFunctionGetItemsValue, cb_id, footstone::value::HippyValue(kError),
                   params);
    };
    storage_module_->GetItemsValue(buffer, success_callback, fail_callback);
  } else if (func == kStroageFunctionSetItemsValue) {
    auto success_callback = [WEAK_THIS, scope, cb_id](const footstone::value::HippyValue& params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kStorageModule, kStroageFunctionSetItemsValue, cb_id, footstone::value::HippyValue(kSuccess),
                   params);
    };
    auto fail_callback = [WEAK_THIS, scope, cb_id](const footstone::value::HippyValue& params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kStorageModule, kStroageFunctionSetItemsValue, cb_id, footstone::value::HippyValue(kError),
                   params);
    };
    storage_module_->SetItemsValue(buffer, success_callback, fail_callback);
  } else if (func == kStroageFunctionRemoveItems) {
    auto success_callback = [WEAK_THIS, scope, cb_id](const footstone::value::HippyValue& params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kStorageModule, kStroageFunctionRemoveItems, cb_id, footstone::value::HippyValue(kSuccess),
                   params);
    };
    auto fail_callback = [WEAK_THIS, scope, cb_id](const footstone::value::HippyValue& params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kStorageModule, kStroageFunctionRemoveItems, cb_id, footstone::value::HippyValue(kError),
                   params);
    };
    storage_module_->RemoveItems(buffer, success_callback, fail_callback);
  } else if (func == kStroageFunctionGetAllItemsKey) {
    auto success_callback = [WEAK_THIS, scope, cb_id](const footstone::value::HippyValue& params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kStorageModule, kStroageFunctionRemoveItems, cb_id, footstone::value::HippyValue(kSuccess),
                   params);
    };
    auto fail_callback = [WEAK_THIS, scope, cb_id](const footstone::value::HippyValue& params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kStorageModule, kStroageFunctionRemoveItems, cb_id, footstone::value::HippyValue(kError),
                   params);
    };
    storage_module_->GetAllItemsKey(success_callback, fail_callback);
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

void ModuleDispatcher::WebsocketModuleHandle(const std::shared_ptr<Scope>& scope, const string_view& func,
                                             const string_view& cb_id, const footstone::value::HippyValue& buffer) {
  if (websocket_module_ == nullptr) return;
  if (func == kWebsocketFunctionConnect) {
    auto callback = [WEAK_THIS, scope, cb_id](footstone::value::HippyValue params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kWebsocketModule, kWebsocketFunctionConnect, cb_id, footstone::value::HippyValue(kSuccess),
                   params);
    };
    websocket_module_->Connect(scope, buffer, callback);
  } else if (func == kWebsocketFunctionClose) {
    websocket_module_->Disconnect(buffer);
  } else if (func == kWebsocketFunctionSend) {
    websocket_module_->Send(buffer);
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

void ModuleDispatcher::NetInfoModuleHandle(const std::shared_ptr<Scope>& scope, const string_view& func,
                                           const string_view& cb_id, const footstone::value::HippyValue& buffer) {
  if (net_info_module_ == nullptr) return;
  if (func == kNetInfoFunctionGetCurrentConnectivity) {
    auto callback = [WEAK_THIS, scope, cb_id](footstone::value::HippyValue params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kNetInfoModule, kNetInfoFunctionGetCurrentConnectivity, cb_id,
                   footstone::value::HippyValue(kSuccess), params);
    };
    net_info_module_->GetCurrentConnectivity(callback);
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

void ModuleDispatcher::NetworkModuleHandle(const std::shared_ptr<Scope>& scope, const string_view& func,
                                           const string_view& cb_id, const footstone::value::HippyValue& buffer) {
  if (network_module_ == nullptr) return;
  if (func == kNetworkFunctionFetch) {
    auto callback = [WEAK_THIS, scope, cb_id](footstone::value::HippyValue params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kNetworkModule, kNetworkFunctionFetch, cb_id, footstone::value::HippyValue(kSuccess), params);
    };
    auto uri_loader = scope->GetUriLoader();
    if (uri_loader.lock()) network_module_->Fetch(uri_loader.lock(), buffer, callback);
  } else if (func == kNetworkFunctionGetCookie) {
    auto callback = [WEAK_THIS, scope, cb_id](footstone::value::HippyValue params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kNetworkModule, kNetworkFunctionGetCookie, cb_id, footstone::value::HippyValue(kSuccess),
                   params);
    };
    network_module_->GetCookie(buffer, callback);
  } else if (func == kNetworkFunctionSetCookie) {
    network_module_->SetCookie(buffer);
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

void ModuleDispatcher::ClipboardModuleHandle(const std::shared_ptr<Scope>& scope, const string_view& func,
                                             const string_view& cb_id, const footstone::value::HippyValue& buffer) {
  if (clipboard_module_ == nullptr) return;
  if (func == kClipboardFunctionGetString) {
    auto callback = [WEAK_THIS, scope, cb_id](footstone::value::HippyValue params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kClipboardModule, kClipboardFunctionGetString, cb_id, footstone::value::HippyValue(kSuccess),
                   params);
    };
    clipboard_module_->GetString(callback);
  } else if (func == kClipboardFunctionSetString) {
    clipboard_module_->SetString(buffer);
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

void ModuleDispatcher::ImageLoaderModuleHandle(const std::shared_ptr<Scope>& scope, const string_view& func,
                                               const string_view& cb_id, const footstone::value::HippyValue& buffer) {
  if (image_loader_module_ == nullptr) return;
  if (func == kImageLoaderFunctionGetSize) {
    auto callback = [WEAK_THIS, scope, cb_id](footstone::value::HippyValue params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(scope, kImageLoaderModule, kImageLoaderFunctionGetSize, cb_id,
                   footstone::value::HippyValue(kSuccess), params);
    };
    auto uri_loader = scope->GetUriLoader();
    if (uri_loader.lock()) image_loader_module_->GetSize(uri_loader.lock(), buffer, callback);
  } else if (func == kImageLoaderFunctionPrefetch) {
    auto uri_loader = scope->GetUriLoader();
    if (uri_loader.lock()) image_loader_module_->Prefetch(uri_loader.lock(), buffer);
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

}  // namespace framework
}  // namespace windows
}  // namespace hippy
