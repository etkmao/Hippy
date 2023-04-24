#include "driver/driver.h"

#include <iostream>

#include "driver/runtime/v8/v8_bridge_utils.h"
#include "driver/scope.h"
#include "footstone/string_view.h"
#include "vfs/handler/asset_handler.h"
#include "vfs/handler/http_handler.h"
#include "vfs/vfs_manager.h"
#ifdef ENABLE_INSPECTOR
#include "devtools/vfs/devtools_handler.h"
#endif  // ENABLE_INSPECTOR

constexpr uint8_t kDispatcherSlot = 3;

namespace hippy {
inline namespace framework {
inline namespace windows {

constexpr char kHippyCurrentDirectoryKey[] = "__HIPPYCURDIR__";
constexpr char kAssetSchema[] = "asset";
constexpr char kHttpSchema[] = "http";
constexpr char kHttpsSchema[] = "https";

Driver::Driver() : module_dispatcher_(std::make_shared<hippy::ModuleDispatcher>()) {}

bool Driver::Initialize(std::shared_ptr<Context> context, std::shared_ptr<DomManager> dom_manager,
                        std::shared_ptr<RootNode> root_node, uint32_t devtools_id) {
  if (module_dispatcher_ == nullptr) return false;
  module_dispatcher_->Initial();

  std::shared_ptr<hippy::V8VMInitParam> param = std::make_shared<hippy::V8VMInitParam>();
  if (config_->GetInitalHeapSize() != kInvalidInitialHeapSize)
    param->initial_heap_size_in_bytes = config_->GetInitalHeapSize();
  if (config_->GetMaximumHeapSize() != kInvalidMaximumHeapSize)
    param->maximum_heap_size_in_bytes = config_->GetMaximumHeapSize();
  V8BridgeUtils::SetOnThrowExceptionToJS([WEAK_THIS](const std::shared_ptr<Runtime>& runtime,
                                                     const hippy::driver::napi::Ctx::string_view& desc,
                                                     const hippy::driver::napi::Ctx::string_view& stack) {
    DEFINE_AND_CHECK_SELF(Driver)
    if (self->exception_handler_) self->exception_handler_(runtime, desc, stack);
  });
  scope_callback_ = [](void*) { FOOTSTONE_LOG(INFO) << "run scope cb"; };

  JsCallback js_callback = [](CallbackInfo& info, void* data) {
    int32_t runtime_id = reinterpret_cast<int32_t>(data);
    std::shared_ptr<Runtime> runtime = Runtime::Find(runtime_id);
    auto module_dispatcher = std::any_cast<std::shared_ptr<ModuleDispatcher>>(runtime->GetData(kDispatcherSlot));
    module_dispatcher->Dispatcher(info, runtime);
  };

  auto loader = context->GetUriLoader();
  auto& work_manager = loader->GetWorkerManager();

  runtime_id_ = hippy::V8BridgeUtils::InitInstance(
      config_->GetEnabelV8Serialization(), config_->GetDevModule(), config_->GetGlobalConfig(), config_->GetGroupId(),
      work_manager, dom_manager->GetTaskRunner(), param, nullptr, scope_callback_, js_callback, devtools_id);

  auto runtime = hippy::Runtime::Find(runtime_id_);
  if (runtime == nullptr) return false;
  auto scope = runtime->GetScope();
  scope->SetDomManager(dom_manager);
  runtime->GetScope()->SetRootNode(root_node);
  runtime->SetData(kDispatcherSlot, module_dispatcher_);


  return true;
}

bool Driver::RunScriptFromUri(string_view uri, std::shared_ptr<hippy::vfs::UriLoader> uri_loader) {
  auto runtime = hippy::Runtime::Find(runtime_id_);
  if (runtime == nullptr) {
    FOOTSTONE_DLOG(WARNING) << "RunScriptFromUri, runtime invalid";
    return false;
  }

  const string_view code_cache_dir = config_->GetCodeCacheDirectory();
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

void Driver::LoadInstance(uint32_t root_id) {
  hippy::V8BridgeUtils::byte_string byte_string = config_->LoadInstanceString(root_id);
  hippy::V8BridgeUtils::LoadInstance(runtime_id_, std::move(byte_string));
}

}  // namespace windows
}  // namespace framework
}  // namespace hippy
