#include "driver/driver.h"

#include <Windows.h>
#include <sstream>

#include "driver/runtime/v8/v8_bridge_utils.h"
#include "driver/scope.h"
#include "footstone/string_view_utils.h"
#include "vfs/handler/asset_handler.h"
#include "vfs/handler/http_handler.h"

#ifdef ENABLE_INSPECTOR
#  include "devtools/vfs/devtools_handler.h"
#endif

namespace hippy {
inline namespace windows {

constexpr char kHippyCurrentDirectoryKey[] = "__HIPPYCURDIR__";
constexpr char kAssetSchema[] = "asset";
constexpr char kHttpSchema[] = "http";
constexpr char kHttpsSchema[] = "https";
constexpr uint8_t kDispatcherSlot = 3;

Driver::Driver() : module_dispatcher_(std::make_shared<hippy::ModuleDispatcher>()) {}

static std::shared_ptr<hippy::V8VMInitParam> CreateV8VMInitParam(const std::shared_ptr<Config>& config) {
  std::shared_ptr<hippy::V8VMInitParam> v8_init_param = std::make_shared<hippy::V8VMInitParam>();
  if (config->GetJsEngine()->GetInitalHeapSize() != kInvalidInitialHeapSize)
    v8_init_param->initial_heap_size_in_bytes = config->GetJsEngine()->GetInitalHeapSize();
  if (config->GetJsEngine()->GetMaximumHeapSize() != kInvalidMaximumHeapSize)
    v8_init_param->maximum_heap_size_in_bytes = config->GetJsEngine()->GetMaximumHeapSize();
  return v8_init_param;
}

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

bool Driver::Initialize(const std::shared_ptr<Config>& config, const std::shared_ptr<DomManager>& dom_manager,
                        const std::shared_ptr<RootNode>& root_node, const std::shared_ptr<UriLoader>& uri_loader,
                        const uint32_t devtools_id) {
  if (module_dispatcher_ == nullptr) return false;
  module_dispatcher_->Initial(config);

  RegisterExceptionHandler();

  // init v8 instance
  bool enable_v8_serialization = config->GetJsEngine()->GetEnableV8Serialization();
  bool is_development_module = config->GetDebug()->GetDevelopmentModule();
  string_view global_config = CreateGlobalConfig(config);
  uint64_t group_id = config->GetJsEngine()->GetGroupId();
  auto& work_manager = uri_loader->GetWorkerManager();
  auto task_runner = dom_manager->GetTaskRunner();
  std::shared_ptr<hippy::V8VMInitParam> v8_init_param = CreateV8VMInitParam(config);
  ScopeCallBack scope_create_callback = [WEAK_THIS](void* data) {
    DEFINE_AND_CHECK_SELF(Driver)
    FOOTSTONE_LOG(INFO) << "run scope cb";
    auto scope_callback = self->GetScopeCallBack();
    if (scope_callback) scope_callback(data);
  };
  JsCallback js_callback = [](CallbackInfo& info, void* data) {
    int32_t runtime_id = reinterpret_cast<int32_t>(data);
    std::shared_ptr<Runtime> runtime = Runtime::Find(runtime_id);
    auto module_dispatcher = std::any_cast<std::shared_ptr<ModuleDispatcher>>(runtime->GetData(kDispatcherSlot));
    module_dispatcher->Dispatcher(info, runtime);
  };

  runtime_id_ = hippy::V8BridgeUtils::InitInstance(enable_v8_serialization, is_development_module, global_config,
                                                   group_id, work_manager, task_runner, v8_init_param, nullptr,
                                                   scope_create_callback, js_callback, devtools_id);

  // runtime bind
  auto runtime = hippy::Runtime::Find(runtime_id_);
  if (runtime == nullptr) return false;
  auto scope = runtime->GetScope();
  scope->SetDomManager(dom_manager);
  runtime->GetScope()->SetRootNode(root_node);
  runtime->SetData(kDispatcherSlot, module_dispatcher_);

  return true;
}

void Driver::RegisterExceptionHandler() {
  V8BridgeUtils::SetOnThrowExceptionToJS([WEAK_THIS](const std::shared_ptr<Runtime>& runtime,
                                                     const hippy::driver::napi::Ctx::string_view& desc,
                                                     const hippy::driver::napi::Ctx::string_view& stack) {
    DEFINE_AND_CHECK_SELF(Driver)
    if (self->exception_handler_) self->exception_handler_(runtime, desc, stack);
  });
}

void Driver::LoadInstance(std::string& load_instance_message) {
  hippy::V8BridgeUtils::LoadInstance(runtime_id_, std::move(load_instance_message));
}

bool Driver::RunScriptFromUri(string_view uri, const std::shared_ptr<UriLoader>& uri_loader,
                              const std::shared_ptr<Config>& config) {
  auto runtime = hippy::Runtime::Find(runtime_id_);
  if (runtime == nullptr) {
    FOOTSTONE_DLOG(WARNING) << "RunScriptFromUri, runtime invalid";
    return false;
  }

  const string_view code_cache_dir = config->GetJsEngine()->GetCodeCacheDirectory();
  auto pos = footstone::stringview::StringViewUtils::FindLastOf(uri, EXTEND_LITERAL('/'));
  size_t len = footstone::stringview::StringViewUtils::GetLength(uri);
  footstone::stringview::StringViewUtils::string_view script_name =
      footstone::stringview::StringViewUtils::SubStr(uri, pos + 1, len);
  footstone::stringview::StringViewUtils::string_view base_path =
      footstone::stringview::StringViewUtils::SubStr(uri, 0, pos + 1);
  FOOTSTONE_DLOG(INFO) << "runScriptFromUri uri = " << uri << ", script_name = " << script_name
                       << ", base_path = " << base_path << ", code_cache_dir = " << code_cache_dir;

  auto runner = runtime->GetEngine()->GetJsTaskRunner();
  auto ctx = runtime->GetScope()->GetContext();
  runner->PostTask([ctx, base_path] {
    if (ctx != nullptr) {
      auto key = ctx->CreateString(kHippyCurrentDirectoryKey);
      auto value = ctx->CreateString(base_path);
      auto global = ctx->GetGlobalObject();
      ctx->SetProperty(global, key, value);
    }
  });

  runtime->GetScope()->SetUriLoader(uri_loader);
  auto http_handler = std::make_shared<hippy::vfs::HttpHandler>();
  http_handler->SetWorkerTaskRunner(runtime->GetEngine()->GetWorkerTaskRunner());
  uri_loader->RegisterUriHandler(kHttpSchema, http_handler);
  uri_loader->RegisterUriHandler(kHttpsSchema, http_handler);
  auto asset_handler = std::make_shared<hippy::vfs::AssetHandler>();
  asset_handler->SetWorkerTaskRunner(runtime->GetEngine()->GetWorkerTaskRunner());
  uri_loader->RegisterUriHandler(kAssetSchema, asset_handler);

#ifdef ENABLE_INSPECTOR
  auto devtools_data_source = runtime->GetDevtoolsDataSource();
  if (devtools_data_source) {
    auto network_notification = devtools_data_source->GetNotificationCenter()->network_notification;
    auto devtools_handler = std::make_shared<hippy::devtools::DevtoolsHandler>();
    devtools_handler->SetNetworkNotification(network_notification);
    uri_loader->RegisterUriInterceptor(devtools_handler);
  }
#endif
  auto func = [runtime, script_name, code_cache_dir, uri] {
    FOOTSTONE_DLOG(INFO) << "runScriptFromUri enter";
    bool flag = hippy::V8BridgeUtils::RunScript(runtime, script_name, false, code_cache_dir, uri, false);
  };
  runner->PostTask(std::move(func));

  return true;
}

}  // namespace windows
}  // namespace hippy
