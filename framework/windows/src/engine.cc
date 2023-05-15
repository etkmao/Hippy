#include "engine.h"

#include "footstone/macros.h"
#include "footstone/serializer.h"
#include "footstone/worker_impl.h"

namespace hippy {
inline namespace windows {
inline namespace framework {

constexpr uint32_t kPoolSize = 2;
constexpr char kDomWorkerName[] = "dom_worker";
constexpr char kDomRunnerName[] = "dom_task_runner";

Engine::Engine(std::shared_ptr<hippy::Config>& config) : config_(config) {}

std::string Engine::CreateLoadInstanceMessage(uint32_t root_id) {
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

std::string Engine::CreateRemoteUri(const std::shared_ptr<Config>& config) {
  std::string host = config->GetDebug()->GetHost();
  std::string bundle_name = config->GetDebug()->GetBundleName();
  return "http://" + host + "/" + bundle_name;
}

void Engine::Initialize() {
  CreateDriver();
  CreateDomManager();
  CreateTdfRenderManager(config_->GetDensity());
  CreateRootNode(config_->GetRootId(), config_->GetDensity());
  CreateUriLoader();
  CreateDevtools();
  Bind();
}

void Engine::CreateDomManager() {
  dom_manager_ = std::make_shared<hippy::DomManager>();
  auto worker = std::make_shared<footstone::WorkerImpl>(kDomWorkerName, false);
  worker->Start();
  auto runner = std::make_shared<footstone::TaskRunner>(kDomRunnerName);
  runner->SetWorker(worker);
  worker->Bind({runner});
  dom_manager_->SetTaskRunner(runner);
  dom_manager_->SetWorker(worker);
  return;
}

void Engine::CreateTdfRenderManager(const float density) {
  tdf_render_manager_ = std::make_shared<TDFRenderManager>();
  tdf_render_manager_->SetDensity(density);
  hippy::InitNodeCreator();
}

void Engine::CreateDriver() { driver_ = std::make_shared<hippy::Driver>(); }

void Engine::CreateRootNode(const uint32_t root_id, const float density) {
  root_node_ = std::make_shared<hippy::RootNode>(root_id);
  auto& persistent_map = hippy::RootNode::PersistentMap();
  auto flag = persistent_map.Insert(root_id, root_node_);
  FOOTSTONE_DCHECK(flag);
  auto layout_node = root_node_->GetLayoutNode();
  layout_node->SetScaleFactor(density);
}

void Engine::CreateDevtools() {
  std::string ws_url = "ws://127.0.0.1:38989/debugger-proxy?role=android_client&clientId=%s&hash=%s&contextName=%s";
  auto worker_manager = std::make_shared<footstone::WorkerManager>(kPoolSize);
  devtools_data_source_ = std::make_shared<hippy::devtools::DevtoolsDataSource>(ws_url, worker_manager);
}

void Engine::CreateUriLoader() { uri_loader_ = std::make_shared<UriLoader>(); }

void Engine::Bind() {
  // Render manager bind Dom manager
  tdf_render_manager_->SetDomManager(dom_manager_);

  // Dom Manager bind Render Manager
  dom_manager_->SetRenderManager(tdf_render_manager_);

  // Root Node bind Dom Manager
  root_node_->SetDomManager(dom_manager_);

  // Devtools bind Dom Manager and Root Node
  devtools_data_source_->Bind(dom_manager_);
  devtools_data_source_->SetRootNode(root_node_);

  // Render Manager Bind Uri Loader
  tdf_render_manager_->SetUriLoader(uri_loader_);
  tdf_render_manager_->RegisterShell(config_->GetRootId(), config_->GetShell());

  // Init driver to create V8 JS Engine
  driver_->Initialize(config_, dom_manager_, root_node_, uri_loader_, devtools_data_source_);

  auto scope_initialized_callback = [WEAK_THIS](std::shared_ptr<Scope> scope) {
    DEFINE_AND_CHECK_SELF(Engine)
    scope->SetDomManager(self->dom_manager_);
    scope->SetRootNode(self->root_node_);
    scope->SetUriLoader(self->uri_loader_);
    auto root_id = self->root_node_->GetId();
    auto scope_initialized_callback = self->GetScopeInitializedCallBack();
    if (scope_initialized_callback) scope_initialized_callback(root_id);
  };
  driver_->ScopeInitializedCallBack(scope_initialized_callback);
}

bool Engine::RunScriptFromUri(const string_view& uri) {
  if (driver_ == nullptr) return false;
  return driver_->RunScriptFromUri(uri, uri_loader_, config_);
}

void Engine::LoadInstance(std::string& load_instance_message) { driver_->LoadInstance(load_instance_message); }

void Engine::ReloadInstance(uint32_t new_root_id) {
  if (driver_) {
    auto callback = [WEAK_THIS, new_root_id]() {
      FOOTSTONE_DLOG(INFO) << "reload instance call begin";
      DEFINE_AND_CHECK_SELF(Engine)
      auto config = self->config_;
      config->SetRootId(new_root_id);
      self->CreateDriver();
      self->CreateTdfRenderManager(config->GetDensity());
      self->CreateRootNode(config->GetRootId(), config->GetDensity());

      self->tdf_render_manager_->SetDomManager(self->dom_manager_);
      self->dom_manager_->SetRenderManager(self->tdf_render_manager_);
      self->root_node_->SetDomManager(self->dom_manager_);
      self->tdf_render_manager_->SetUriLoader(self->uri_loader_);
      self->tdf_render_manager_->RegisterShell(config->GetRootId(), config->GetShell());

      self->driver_->Initialize(config, self->dom_manager_, self->root_node_, self->uri_loader_,
                                self->devtools_data_source_, true);
      auto scope_initialized_callback = [weak_this = self->weak_from_this()](std::shared_ptr<Scope> scope) {
        DEFINE_AND_CHECK_SELF(Engine)
        scope->SetDomManager(self->dom_manager_);
        scope->SetRootNode(self->root_node_);
        scope->SetUriLoader(self->uri_loader_);
        auto root_id = self->root_node_->GetId();
        auto scope_initialized_callback = self->GetScopeInitializedCallBack();
        if (scope_initialized_callback) scope_initialized_callback(root_id);
      };
      self->driver_->ScopeInitializedCallBack(scope_initialized_callback);
    };
    driver_->ReloadInstance(root_node_->GetId(), callback);
  }
}

}  // namespace framework
}  // namespace windows
}  // namespace hippy
