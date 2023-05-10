#include "driver/module_dispatcher.h"

#include <utility>

#include "config.h"
#include "driver/napi/v8/v8_ctx.h"
#include "driver/runtime/v8/v8_bridge_utils.h"
#include "driver/scope.h"
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

void ModuleDispatcher::Dispatcher(const CallbackInfo& info, const std::shared_ptr<Runtime>& runtime) {
  FOOTSTONE_DLOG(INFO) << "Call Windows Module";
  auto context = runtime->GetScope()->GetContext();

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
    if (runtime->IsEnableV8Serialization()) {
      auto v8_ctx = std::static_pointer_cast<hippy::napi::V8Ctx>(context);
      buffer_data = v8_ctx->GetSerializationBuffer(info[3], runtime->GetBuffer());
    } else {
      string_view json;
      auto flag = context->GetValueJson(info[3], &json);
      FOOTSTONE_DCHECK(flag);
      buffer_data = footstone::StringViewUtils::ToStdString(
          footstone::StringViewUtils::ConvertEncoding(json, string_view::Encoding::Utf8).utf8_value());
    }

    auto v8_ctx = std::static_pointer_cast<hippy::napi::V8Ctx>(context);
    buffer_data = v8_ctx->GetSerializationBuffer(info[3], runtime->GetBuffer());
    footstone::Deserializer deserializer(reinterpret_cast<const uint8_t*>(buffer_data.c_str()), buffer_data.length());
    deserializer.ReadHeader();
    deserializer.ReadValue(value);
  }

  int32_t transfer_type = 0;
  if (info[4]) {
    context->GetValueNumber(info[4], &transfer_type);
  }

  FOOTSTONE_DLOG(INFO) << "module name = " << module_name << ", function name = " << fn_name
                       << ", parameter = " << buffer_data;
  if (module_name == kClipboardModule) {
    ClipboardModuleHandle(fn_name, runtime->GetId(), cb_id_str, value);
  } else if (module_name == kImageLoaderModule) {
    auto loader = runtime->GetScope()->GetUriLoader().lock();
    if (loader != nullptr) ImageLoaderModuleHandle(loader, fn_name, runtime->GetId(), cb_id_str, value);
  } else if (module_name == kNetInfoModule) {
    NetInfoModuleHandle(fn_name, runtime->GetId(), cb_id_str, value);
  } else if (module_name == kNetworkModule) {
    auto loader = runtime->GetScope()->GetUriLoader().lock();
    if (loader != nullptr) NetworkModuleHandle(loader, fn_name, runtime->GetId(), cb_id_str, value);
  } else if (module_name == kWebsocketModule) {
    WebsocketModuleHandle(fn_name, runtime->GetId(), cb_id_str, value);
  } else if (module_name == kStorageModule) {
    StorageModuleHandle(fn_name, runtime->GetId(), cb_id_str, value);
  } else {
    FOOTSTONE_LOG(WARNING) << "module " << module_name << " is not support !!!";
  }
}

void ModuleDispatcher::CallJs(const uint32_t runtime_id, const std::string& module_name,
                              const std::string& function_name, const string_view& callback_id,
                              const footstone::value::HippyValue& callback_parameters) {
  serializer_.Release();
  serializer_.WriteHeader();
  footstone::HippyValue::HippyValueObjectType object;
  object.insert({"result", footstone::HippyValue(kSuccess)});
  object.insert({"moduleName", footstone::HippyValue(module_name)});
  object.insert({"moduleFunc", footstone::HippyValue(function_name)});
  object.insert({"callId", footstone::HippyValue(callback_id.latin1_value())});
  object.insert({"params", callback_parameters});
  serializer_.WriteValue(footstone::HippyValue(object));
  std::pair<uint8_t*, size_t> buffer = serializer_.Release();
  byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
  auto call_js_callback = [](CALL_FUNCTION_CB_STATE state, const string_view& msg) { FOOTSTONE_DLOG(INFO) << msg; };
  auto on_js_runner = []() {};
  V8BridgeUtils::CallJs(u"callBack", runtime_id, call_js_callback, buffer_data, on_js_runner);
}

void ModuleDispatcher::StorageModuleHandle(const string_view& func, int32_t runtime_id, const string_view& cb_id,
                                           const footstone::value::HippyValue& value) {
  if (storage_module_ == nullptr) return;
  if (func == kStroageFunctionGetItemsValue) {
    std::vector<std::string> keys;
    auto ret = ParserParameters(value, keys);
    FOOTSTONE_DCHECK(ret);
    if (ret) {
      auto callback = [runtime_id, cb_id](StorageResponse response, std::unordered_map<std::string, std::string> kvs) {
        footstone::string_view action_name(u"callBack");
        footstone::Serializer serializer;
        serializer.WriteHeader();
        footstone::HippyValue::HippyValueObjectType object;

        if (response.GetRetCode() == RetCode::Success) {
          object.insert({"result", footstone::HippyValue(kSuccess)});
          if (kvs.size() == 0) {
            object.insert({"params", footstone::HippyValue::Null()});
          } else {
            footstone::HippyValue::HippyValueArrayType array;
            for (const auto& kv : kvs) {
              footstone::HippyValue::HippyValueArrayType kv_array;
              kv_array.push_back(footstone::HippyValue(kv.first));
              kv_array.push_back(footstone::HippyValue(kv.second));
              array.push_back(footstone::HippyValue(kv_array));
            }
            object.insert({"params", footstone::HippyValue(array)});
          }
        } else {
          object.insert({"result", footstone::HippyValue(kError)});
          object.insert({"params", footstone::HippyValue(response.GetErrorMsg())});
        }

        object.insert({"moduleName", footstone::HippyValue(kStorageModule)});
        object.insert({"moduleFunc", footstone::HippyValue(kStroageFunctionGetItemsValue)});
        object.insert({"callId", footstone::HippyValue(cb_id.latin1_value())});
        serializer.WriteValue(footstone::HippyValue(object));
        std::pair<uint8_t*, size_t> buffer = serializer.Release();
        byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
        auto call_js_callback = [](CALL_FUNCTION_CB_STATE state, const string_view& msg) {
          FOOTSTONE_DLOG(INFO) << msg;
        };
        auto on_js_runner = []() {};
        V8BridgeUtils::CallJs(action_name, runtime_id, call_js_callback, buffer_data, on_js_runner);
      };
      storage_module_->GetItemsValue(keys, callback);
    }
  } else if (func == kStroageFunctionSetItemsValue) {
    std::unordered_map<std::string, std::string> kvs;
    auto ret = ParserParameters(value, kvs);
    FOOTSTONE_DCHECK(ret);
    if (ret) {
      auto callback = [runtime_id, cb_id](StorageResponse response) {
        footstone::string_view action_name(u"callBack");
        footstone::Serializer serializer;
        serializer.WriteHeader();
        footstone::HippyValue::HippyValueObjectType object;

        if (response.GetRetCode() == RetCode::Success) {
          object.insert({"result", footstone::HippyValue(kSuccess)});
          object.insert({"params", footstone::HippyValue("sucess")});

        } else {
          object.insert({"result", footstone::HippyValue(kError)});
          object.insert({"params", footstone::HippyValue(response.GetErrorMsg())});
        }

        object.insert({"moduleName", footstone::HippyValue(kStorageModule)});
        object.insert({"moduleFunc", footstone::HippyValue(kStroageFunctionSetItemsValue)});
        object.insert({"callId", footstone::HippyValue(cb_id.latin1_value())});
        serializer.WriteValue(footstone::HippyValue(object));
        std::pair<uint8_t*, size_t> buffer = serializer.Release();
        byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
        auto call_js_callback = [](CALL_FUNCTION_CB_STATE state, const string_view& msg) {
          FOOTSTONE_DLOG(INFO) << msg;
        };
        auto on_js_runner = []() {};
        V8BridgeUtils::CallJs(action_name, runtime_id, call_js_callback, buffer_data, on_js_runner);
      };
      storage_module_->SetItemsValue(kvs, callback);
    }
  } else if (func == kStroageFunctionRemoveItems) {
    std::vector<std::string> keys;
    auto ret = ParserParameters(value, keys);
    FOOTSTONE_DCHECK(ret);
    if (ret) {
      auto callback = [runtime_id, cb_id](StorageResponse response) {
        footstone::string_view action_name(u"callBack");
        footstone::Serializer serializer;
        serializer.WriteHeader();
        footstone::HippyValue::HippyValueObjectType object;

        if (response.GetRetCode() == RetCode::Success) {
          object.insert({"result", footstone::HippyValue(kSuccess)});
          object.insert({"params", footstone::HippyValue::Null()});
        } else {
          object.insert({"result", footstone::HippyValue(kError)});
          object.insert({"params", footstone::HippyValue(response.GetErrorMsg())});
        }
        object.insert({"moduleName", footstone::HippyValue(kStorageModule)});
        object.insert({"moduleFunc", footstone::HippyValue(kStroageFunctionSetItemsValue)});
        object.insert({"callId", footstone::HippyValue(cb_id.latin1_value())});

        serializer.WriteValue(footstone::HippyValue(object));
        std::pair<uint8_t*, size_t> buffer = serializer.Release();
        byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
        auto call_js_callback = [](CALL_FUNCTION_CB_STATE state, const string_view& msg) {
          FOOTSTONE_DLOG(INFO) << msg;
        };
        auto on_js_runner = []() {};
        V8BridgeUtils::CallJs(action_name, runtime_id, call_js_callback, buffer_data, on_js_runner);
      };
      storage_module_->RemoveItems(keys, callback);
    }
  } else if (func == kStroageFunctionGetAllItemsKey) {
    auto callback = [runtime_id, cb_id](StorageResponse response, std::vector<std::string> values) {
      footstone::string_view action_name(u"callBack");
      footstone::Serializer serializer;
      serializer.WriteHeader();
      footstone::HippyValue::HippyValueObjectType object;

      if (response.GetRetCode() == RetCode::Success) {
        object.insert({"result", footstone::HippyValue(kSuccess)});
        footstone::HippyValue::HippyValueArrayType array;
        for (const auto& value : values) {
          array.push_back(footstone::HippyValue(value));
        }
        object.insert({"params", footstone::HippyValue(array)});
      } else {
        object.insert({"result", footstone::HippyValue(kError)});
        object.insert({"params", footstone::HippyValue(response.GetErrorMsg())});
      }
      object.insert({"moduleName", footstone::HippyValue(kStorageModule)});
      object.insert({"moduleFunc", footstone::HippyValue(kStroageFunctionSetItemsValue)});
      object.insert({"callId", footstone::HippyValue(cb_id.latin1_value())});

      serializer.WriteValue(footstone::HippyValue(object));
      std::pair<uint8_t*, size_t> buffer = serializer.Release();
      byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
      auto call_js_callback = [](CALL_FUNCTION_CB_STATE state, const string_view& msg) { FOOTSTONE_DLOG(INFO) << msg; };
      auto on_js_runner = []() {};
      V8BridgeUtils::CallJs(action_name, runtime_id, call_js_callback, buffer_data, on_js_runner);
    };
    storage_module_->GetAllItemsKey(callback);
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

void ModuleDispatcher::WebsocketModuleHandle(const string_view& func, int32_t runtime_id, const string_view& cb_id,
                                             const footstone::value::HippyValue& buffer) {
  if (websocket_module_ == nullptr) return;
  if (func == kWebsocketFunctionConnect) {
    auto callback = [runtime_id, cb_id](footstone::value::HippyValue response) {
      footstone::Serializer serializer;
      serializer.WriteHeader();
      footstone::HippyValue::HippyValueObjectType object;
      object.insert({"result", footstone::HippyValue(kSuccess)});
      object.insert({"moduleName", footstone::HippyValue(kWebsocketModule)});
      object.insert({"moduleFunc", footstone::HippyValue(kWebsocketFunctionConnect)});
      object.insert({"callId", footstone::HippyValue(cb_id.latin1_value())});
      object.insert({"params", response});

      serializer.WriteValue(footstone::HippyValue(object));
      std::pair<uint8_t*, size_t> buffer = serializer.Release();
      byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
      auto call_js_callback = [](CALL_FUNCTION_CB_STATE state, const string_view& msg) { FOOTSTONE_DLOG(INFO) << msg; };
      auto on_js_runner = []() {};
      V8BridgeUtils::CallJs(u"callBack", runtime_id, call_js_callback, buffer_data, on_js_runner);
    };
    websocket_module_->Connect(buffer, runtime_id, callback);
  } else if (func == kWebsocketFunctionClose) {
    websocket_module_->Disconnect(buffer, runtime_id);
  } else if (func == kWebsocketFunctionSend) {
    websocket_module_->Send(buffer, runtime_id);
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

void ModuleDispatcher::NetInfoModuleHandle(const string_view& func, int32_t runtime_id, const string_view& cb_id,
                                           const footstone::value::HippyValue& buffer) {
  if (net_info_module_ == nullptr) return;
  if (func == kNetInfoFunctionGetCurrentConnectivity) {
    auto callback = [runtime_id, cb_id](footstone::value::HippyValue params) {
      footstone::Serializer serializer;
      serializer.WriteHeader();
      footstone::HippyValue::HippyValueObjectType object;
      object.insert({"result", footstone::HippyValue(kSuccess)});
      object.insert({"moduleName", footstone::HippyValue(kWebsocketModule)});
      object.insert({"moduleFunc", footstone::HippyValue(kWebsocketFunctionConnect)});
      object.insert({"callId", footstone::HippyValue(cb_id.latin1_value())});
      object.insert({"params", params});

      serializer.WriteValue(footstone::HippyValue(object));
      std::pair<uint8_t*, size_t> buffer = serializer.Release();
      byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
      auto call_js_callback = [](CALL_FUNCTION_CB_STATE state, const string_view& msg) { FOOTSTONE_DLOG(INFO) << msg; };
      auto on_js_runner = []() {};
      V8BridgeUtils::CallJs(u"callBack", runtime_id, call_js_callback, buffer_data, on_js_runner);
    };
    net_info_module_->GetCurrentConnectivity(callback);
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

void ModuleDispatcher::NetworkModuleHandle(const std::shared_ptr<UriLoader>& uri_loader, const string_view& func,
                                           int32_t runtime_id, const string_view& cb_id,
                                           const footstone::value::HippyValue& buffer) {
  if (network_module_ == nullptr) return;
  if (func == kNetworkFunctionFetch) {
    auto callback = [runtime_id, cb_id](footstone::value::HippyValue params) {
      footstone::Serializer serializer;
      serializer.WriteHeader();
      footstone::HippyValue::HippyValueObjectType object;
      object.insert({"result", footstone::HippyValue(kSuccess)});
      object.insert({"moduleName", footstone::HippyValue(kNetworkModule)});
      object.insert({"moduleFunc", footstone::HippyValue(kNetworkFunctionFetch)});
      object.insert({"callId", footstone::HippyValue(cb_id.latin1_value())});
      object.insert({"params", params});
      serializer.WriteValue(footstone::HippyValue(object));
      std::pair<uint8_t*, size_t> buffer = serializer.Release();
      byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
      auto call_js_callback = [](CALL_FUNCTION_CB_STATE state, const string_view& msg) { FOOTSTONE_DLOG(INFO) << msg; };
      auto on_js_runner = []() {};
      V8BridgeUtils::CallJs(u"callBack", runtime_id, call_js_callback, buffer_data, on_js_runner);
    };
    network_module_->Fetch(uri_loader, buffer, runtime_id, callback);
  } else if (func == kNetworkFunctionGetCookie) {
    auto callback = [runtime_id, cb_id](footstone::value::HippyValue params) {
      footstone::Serializer serializer;
      serializer.WriteHeader();
      footstone::HippyValue::HippyValueObjectType object;
      object.insert({"result", footstone::HippyValue(kSuccess)});
      object.insert({"moduleName", footstone::HippyValue(kNetworkModule)});
      object.insert({"moduleFunc", footstone::HippyValue(kNetworkFunctionGetCookie)});
      object.insert({"callId", footstone::HippyValue(cb_id.latin1_value())});
      object.insert({"params", params});
      serializer.WriteValue(footstone::HippyValue(object));
      std::pair<uint8_t*, size_t> buffer = serializer.Release();
      byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
      auto call_js_callback = [](CALL_FUNCTION_CB_STATE state, const string_view& msg) { FOOTSTONE_DLOG(INFO) << msg; };
      auto on_js_runner = []() {};
      V8BridgeUtils::CallJs(u"callBack", runtime_id, call_js_callback, buffer_data, on_js_runner);
    };
    network_module_->GetCookie(buffer, runtime_id, callback);
  } else if (func == kNetworkFunctionSetCookie) {
    network_module_->SetCookie(buffer);
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

void ModuleDispatcher::ClipboardModuleHandle(const string_view& func, int32_t runtime_id, const string_view& cb_id,
                                             const footstone::value::HippyValue& buffer) {
  if (clipboard_module_ == nullptr) return;
  if (func == kClipboardFunctionGetString) {
    auto callback = [runtime_id, cb_id](footstone::value::HippyValue params) {
      footstone::Serializer serializer;
      serializer.WriteHeader();
      footstone::HippyValue::HippyValueObjectType object;
      object.insert({"result", footstone::HippyValue(kSuccess)});
      object.insert({"moduleName", footstone::HippyValue(kNetworkModule)});
      object.insert({"moduleFunc", footstone::HippyValue(kNetworkFunctionGetCookie)});
      object.insert({"callId", footstone::HippyValue(cb_id.latin1_value())});
      object.insert({"params", params});
      serializer.WriteValue(footstone::HippyValue(object));
      std::pair<uint8_t*, size_t> buffer = serializer.Release();
      byte_string buffer_data{reinterpret_cast<char*>(buffer.first), buffer.second};
      auto call_js_callback = [](CALL_FUNCTION_CB_STATE state, const string_view& msg) { FOOTSTONE_DLOG(INFO) << msg; };
      auto on_js_runner = []() {};
      V8BridgeUtils::CallJs(u"callBack", runtime_id, call_js_callback, buffer_data, on_js_runner);
    };
    clipboard_module_->GetString(callback);
  } else if (func == kClipboardFunctionSetString) {
    clipboard_module_->SetString(buffer);
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

void ModuleDispatcher::ImageLoaderModuleHandle(const std::shared_ptr<UriLoader>& uri_loader, const string_view& func,
                                               int32_t runtime_id, const string_view& cb_id,
                                               const footstone::value::HippyValue& buffer) {
  if (image_loader_module_ == nullptr) return;
  if (func == kImageLoaderFunctionGetSize) {
    auto callback = [WEAK_THIS, runtime_id, cb_id](footstone::value::HippyValue params) {
      DEFINE_AND_CHECK_SELF(ModuleDispatcher)
      self->CallJs(runtime_id, kImageLoaderModule, kImageLoaderFunctionGetSize, cb_id, params);
    };
    image_loader_module_->GetSize(uri_loader, buffer, callback);
  } else if (func == kImageLoaderFunctionPrefetch) {
    image_loader_module_->Prefetch();
  } else {
    FOOTSTONE_LOG(WARNING) << "function " << func << " is not support !!!";
  }
}

bool ModuleDispatcher::ParserParameters(const footstone::value::HippyValue& value,
                                        std::unordered_map<std::string, std::string>& parsed) {
  footstone::value::HippyValue::HippyValueArrayType parameters;
  bool ret = value.ToArray(parameters);
  if (!ret) return false;
  if (parameters.size() != 1) return false;

  footstone::value::HippyValue::HippyValueArrayType kvs;
  ret = parameters[0].ToArray(kvs);
  if (!ret) return false;
  for (size_t i = 0; i < kvs.size(); i++) {
    footstone::value::HippyValue::HippyValueArrayType kv;
    kvs[i].ToArray(kv);
    if (kv.size() != 2) return false;
    std::string key, value;
    ret = kv[0].ToString(key);
    if (!ret) return false;
    ret = kv[1].ToString(value);
    if (!ret) return false;
    parsed.insert({key, value});
  }

  return true;
}

bool ModuleDispatcher::ParserParameters(const footstone::value::HippyValue& value, std::vector<std::string>& parsed) {
  footstone::value::HippyValue::HippyValueArrayType parameters;
  bool ret = value.ToArray(parameters);
  if (!ret) return false;
  if (parameters.size() != 1) return false;

  footstone::value::HippyValue::HippyValueArrayType keys;
  ret = parameters[0].ToArray(keys);
  if (!ret) return false;
  for (size_t i = 0; i < keys.size(); i++) {
    std::string key;
    ret = keys[0].ToString(key);
    if (!ret) return false;
    parsed.push_back(key);
  }

  return true;
}
}  // namespace framework
}  // namespace windows
}  // namespace hippy
