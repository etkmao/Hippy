#include "engine.h"

#include "footstone/macros.h"
#include "footstone/worker_impl.h"

namespace hippy {
inline namespace windows {
inline namespace framework {

constexpr uint32_t kPoolSize = 2;
constexpr char kDomWorkerName[] = "dom_worker";
constexpr char kDomRunnerName[] = "dom_task_runner";

Engine::Engine(std::shared_ptr<hippy::Config>& config) : config_(std::move(config)) {}

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
  devtools_id_ = devtools::DevtoolsDataSource::Insert(devtools_data_source_);
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
  driver_->Initialize(config_, dom_manager_, root_node_, uri_loader_, devtools_id_);

  auto scope_callback = [WEAK_THIS](void* data) {
    DEFINE_AND_CHECK_SELF(Engine)
    auto scope_callback = self->GetScopeCallBack();
    if (scope_callback) scope_callback(data);
  };
  driver_->SetScopeCallBack(scope_callback);
}

bool Engine::RunScriptFromUri(const string_view& uri) {
  if (driver_ == nullptr) return false;
  return driver_->RunScriptFromUri(uri, uri_loader_, config_);
}

void Engine::LoadInstance(std::string& load_instance_message) { driver_->LoadInstance(load_instance_message); }

}  // namespace framework
}  // namespace windows
}  // namespace hippy
