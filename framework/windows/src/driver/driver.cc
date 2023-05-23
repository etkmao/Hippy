#include "driver/driver.h"

#include <Windows.h>
#include <sstream>

#include "driver/js_driver_utils.h"
#include "driver/vm/v8/v8_vm.h"
#include "footstone/macros.h"
#include "footstone/string_view_utils.h"
#include "vfs/handler/asset_handler.h"
#include "vfs/handler/http_handler.h"

#ifdef ENABLE_INSPECTOR
#  include "devtools/vfs/devtools_handler.h"
#endif

using V8VMInitParam = hippy::V8VMInitParam;
using StringViewUtils = footstone::stringview::StringViewUtils;

namespace hippy {
inline namespace windows {

constexpr char kHippyCurrentDirectoryKey[] = "__HIPPYCURDIR__";
constexpr char kAssetSchema[] = "asset";
constexpr char kHttpSchema[] = "http";
constexpr char kHttpsSchema[] = "https";

Driver::Driver() : module_dispatcher_(std::make_shared<hippy::ModuleDispatcher>()) {}

static string_view CreateGlobalConfig(const std::shared_ptr<Config>& config) {
  std::stringstream ss;
  ss << "{";
  ss << "\"Platform\":{\"OS\":\"windows\",\"Localization\":{\"country\":\"CN\",\"language\":\"zh\",\"direction\":0},"
        "\"NightMode\":false},";

  HWND handle = GetForegroundWindow();
  RECT rect;
  GetWindowRect(handle, &rect);
  int width = rect.right - rect.left;
  int height = rect.bottom - rect.top;
  int screen_width = GetSystemMetrics(SM_CXSCREEN);
  int screen_height = GetSystemMetrics(SM_CYSCREEN);
  ss << "\"Dimensions\": {\"screenPhysicalPixels\": {\"width\":" << screen_width << ", \"height\": " << screen_height
     << ", \"scale\":" << config->GetDensity() << "}, \"windowPhysicalPixels\": {\"width\":" << width
     << ", \"height\": " << height << ", \"scale\":" << config->GetDensity() << "}}";
  ss << "}";
  return string_view(ss.str());
}

// TODO(charleeshen): source path must change
static std::string LoadInstanceMessage(uint32_t root_id) {
  footstone::value::Serializer serializer;
  serializer.WriteHeader();
  footstone::value::HippyValue::HippyValueObjectType obj;
  obj.insert({"name", footstone::value::HippyValue("Demo")});
  obj.insert({"id", footstone::value::HippyValue(root_id)});
  footstone::value::HippyValue::HippyValueObjectType params;
  params.insert({"msgFromNative", footstone::value::HippyValue("Hi js developer,I come from native code !")});
  params.insert({"sourcePath", footstone::value::HippyValue("index.android.js")});
  obj.insert({"params", footstone::value::HippyValue(params)});
  serializer.WriteValue(footstone::value::HippyValue(obj));
  std::pair<uint8_t*, size_t> buffer = serializer.Release();
  std::string byte_string(reinterpret_cast<char*>(buffer.first), buffer.second);
  return byte_string;
}

static std::string UnloadInstanceMessage(uint32_t root_id) {
  footstone::value::Serializer serializer;
  serializer.WriteHeader();
  footstone::value::HippyValue::HippyValueObjectType object;
  object.insert({"id", footstone::value::HippyValue(root_id)});
  serializer.WriteValue(footstone::value::HippyValue(object));
  std::pair<uint8_t*, size_t> buffer = serializer.Release();
  std::string byte_string(reinterpret_cast<char*>(buffer.first), buffer.second);
  return byte_string;
}

bool Driver::Initialize(const std::shared_ptr<Config>& config, const std::shared_ptr<DomManager>& dom_manager,
                        const std::shared_ptr<RootNode>& root_node, const std::shared_ptr<UriLoader>& uri_loader,
                        const std::shared_ptr<DevtoolsDataSource>& devtools_data_source, const bool reload) {
  FOOTSTONE_DCHECK(module_dispatcher_);
  if (module_dispatcher_ == nullptr) return false;
  module_dispatcher_->Initial(config);

  // v8 init parameter
  auto param = std::make_shared<V8VMInitParam>();
  param->enable_v8_serialization = config->GetJsEngine()->GetEnableV8Serialization();
  bool is_debug = config->GetDebug()->GetDevelopmentModule();
  param->is_debug = is_debug;
  if (config->GetJsEngine()->GetInitalHeapSize() != kInvalidInitialHeapSize)
    param->initial_heap_size_in_bytes = config->GetJsEngine()->GetInitalHeapSize();
  if (config->GetJsEngine()->GetMaximumHeapSize() != kInvalidMaximumHeapSize)
    param->maximum_heap_size_in_bytes = config->GetJsEngine()->GetMaximumHeapSize();
  param->uncaught_exception_callback = [WEAK_THIS, is_debug](const std::any& bridge, const string_view& desc,
                                                             const string_view& stack) {
    auto module_dispatcher = std::any_cast<std::shared_ptr<ModuleDispatcher>>(bridge);
    if (module_dispatcher) {
      std::string module_name("ExceptionModule");
      std::string func_name("HandleException");
      std::string desc_u8 = StringViewUtils::ToStdString(
          StringViewUtils::ConvertEncoding(desc, string_view::Encoding::Utf8).utf8_value());
      std::string stack_u8 = StringViewUtils::ToStdString(
          StringViewUtils::ConvertEncoding(stack, string_view::Encoding::Utf8).utf8_value());
      module_dispatcher->Dispatcher(module_name, func_name, desc_u8, stack_u8);
    }
  };
  auto dom_task_runner = dom_manager->GetTaskRunner();
  auto group_id = config->GetJsEngine()->GetGroupId();
  if (param->is_debug) {
    param->devtools_data_source = devtools_data_source;
#ifdef ENABLE_INSPECTOR
    if (devtools_data_source) {
      auto network_notification = devtools_data_source->GetNotificationCenter()->network_notification;
      auto devtools_handler = std::make_shared<hippy::devtools::DevtoolsHandler>();
      devtools_handler->SetNetworkNotification(network_notification);
      // uri_loader->RegisterUriInterceptor(devtools_handler);
    }
#endif
  }

  js_engine_ = JsDriverUtils::CreateEngineAndAsyncInitialize(dom_task_runner, param, group_id);
  auto http_handler = std::make_shared<hippy::vfs::HttpHandler>();
  http_handler->SetWorkerTaskRunner(js_engine_->GetJsTaskRunner());
  uri_loader->RegisterUriHandler(kHttpSchema, http_handler);
  uri_loader->RegisterUriHandler(kHttpsSchema, http_handler);
  auto asset_handler = std::make_shared<hippy::vfs::AssetHandler>();
  asset_handler->SetWorkerTaskRunner(js_engine_->GetJsTaskRunner());
  uri_loader->RegisterUriHandler(kAssetSchema, asset_handler);

  auto call_host_callback = [](CallbackInfo& info, void* data) {
    FOOTSTONE_DLOG(INFO) << "CallHost";
    auto scope_wrapper = reinterpret_cast<ScopeWrapper*>(std::any_cast<void*>(info.GetSlot()));
    auto scope = scope_wrapper->scope.lock();
    FOOTSTONE_CHECK(scope);
    auto module_dispatcher = std::any_cast<std::shared_ptr<ModuleDispatcher>>(scope->GetBridge());
    if (module_dispatcher) module_dispatcher->Dispatcher(info, scope);
  };
  string_view global_config = CreateGlobalConfig(config);
  auto scope_initialized_callback = [WEAK_THIS](std::shared_ptr<Scope> scope) {
    DEFINE_AND_CHECK_SELF(Driver)
    FOOTSTONE_LOG(INFO) << "run scope cb";
    self->SetScope(scope);
    scope->SetBridge(self->module_dispatcher_);
    auto scope_initialized_callback = self->GetScopeInitializedCallBack();
    if (scope_initialized_callback) scope_initialized_callback(scope);
  };
  JsDriverUtils::InitInstance(js_engine_, param, global_config, scope_initialized_callback, call_host_callback);
  return true;
}

bool Driver::RunScriptFromUri(string_view uri, const std::shared_ptr<UriLoader>& uri_loader,
                              const std::shared_ptr<Config>& config) {
  const string_view code_cache_dir = config->GetJsEngine()->GetCodeCacheDirectory();
  auto pos = footstone::stringview::StringViewUtils::FindLastOf(uri, EXTEND_LITERAL('/'));
  size_t len = footstone::stringview::StringViewUtils::GetLength(uri);
  footstone::stringview::StringViewUtils::string_view script_name =
      footstone::stringview::StringViewUtils::SubStr(uri, pos + 1, len);
  footstone::stringview::StringViewUtils::string_view base_path =
      footstone::stringview::StringViewUtils::SubStr(uri, 0, pos + 1);
  FOOTSTONE_DLOG(INFO) << "runScriptFromUri uri = " << uri << ", script_name = " << script_name
                       << ", base_path = " << base_path << ", code_cache_dir = " << code_cache_dir;

  auto runner = scope_->GetTaskRunner();
  auto ctx = scope_->GetContext();
  runner->PostTask([ctx, base_path] {
    if (ctx != nullptr) {
      auto key = ctx->CreateString(kHippyCurrentDirectoryKey);
      auto value = ctx->CreateString(base_path);
      auto global = ctx->GetGlobalObject();
      ctx->SetProperty(global, key, value);
    }
  });

  auto func = [WEAK_THIS, script_name, code_cache_dir, uri] {
    DEFINE_AND_CHECK_SELF(Driver)
    FOOTSTONE_DLOG(INFO) << "runScriptFromUri enter";
    // TODO(charleeshen): code cache use
    auto scope = self->GetScope();
    bool flag = JsDriverUtils::RunScript(scope, script_name, false, code_cache_dir, uri, false);
  };
  runner->PostTask(std::move(func));

  return true;
}

void Driver::LoadInstance(const uint32_t root_id) {
  std::string load_instance_message = LoadInstanceMessage(root_id);
  JsDriverUtils::LoadInstance(scope_, std::move(load_instance_message));
}

void Driver::ReloadInstance(const uint32_t root_id, std::function<void()> callback) {
  std::string unload_instance_message = UnloadInstanceMessage(root_id);
  JsDriverUtils::UnloadInstance(scope_, std::move(unload_instance_message));

  auto new_callback = [callback](bool ret) {
    if (ret) {
      if (callback) callback();
    } else {
      FOOTSTONE_DLOG(INFO) << "reload engine failed !!!";
    }
  };
  auto destroy_callback = [WEAK_THIS, new_callback](bool ret) {
    DEFINE_AND_CHECK_SELF(Driver);
    JsDriverUtils::DestroyInstance(self->js_engine_, self->scope_, new_callback, true);
  };
  module_dispatcher_->AddDestroyListener(root_id, destroy_callback);
}

void Driver::DestroyInstance(const uint32_t root_id, std::function<void()> callback) {
  std::string unload_instance_message = UnloadInstanceMessage(root_id);
  JsDriverUtils::UnloadInstance(scope_, std::move(unload_instance_message));

  auto new_callback = [callback](bool ret) {
    if (ret) {
      if (callback) callback();
    } else {
      FOOTSTONE_DLOG(INFO) << "reload engine failed !!!";
    }
  };
  auto destroy_callback = [WEAK_THIS, new_callback](bool ret) {
    DEFINE_AND_CHECK_SELF(Driver);
    JsDriverUtils::DestroyInstance(self->js_engine_, self->scope_, new_callback, false);
  };
  module_dispatcher_->AddDestroyListener(root_id, destroy_callback);
}

}  // namespace windows
}  // namespace hippy
