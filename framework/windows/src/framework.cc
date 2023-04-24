#include "framework.h"

#include "dom/root_node.h"
#include "footstone/logging.h"
#include "footstone/macros.h"
#include "footstone/worker_impl.h"
#ifdef ENABLE_INSPECTOR
#include "devtools/devtools_data_source.h"
#include "devtools/vfs/devtools_handler.h"
#endif

namespace hippy {
inline namespace framework {
inline namespace windows {

constexpr uint32_t kPoolSize = 2;
constexpr char kDomWorkerName[] = "dom_worker";
constexpr char kDomRunnerName[] = "dom_task_runner";

FrameWork::FrameWork()
    :console_adaptor_(std::make_shared<hippy::Console>("demo", true)){};

// TODO(charleeshen): 需要重构整个工程
bool FrameWork::Initialize(std::shared_ptr<Config> config) {
  CreateDomManager();
  CreateRenderManager();
  CreateDriver();
  auto devtools_id = CreateDevtools();

  if (context_ == nullptr) return false;

  // init render manager
  auto density = context_->GetDensity();
  render_manager_->SetDensity(density);
  render_manager_->SetDomManager(dom_manager_);
  hippy::InitNodeCreator();

  // dom manager bind render manager
  dom_manager_->SetRenderManager(render_manager_);

  // root node bind dom manager
  auto root_node = context_->GetRootNode();
  if (root_node == nullptr) return false;
  root_node->SetDomManager(dom_manager_);

  auto devtools_data_source = devtools::DevtoolsDataSource::Find(devtools_id);
  devtools_data_source->Bind(dom_manager_);
  devtools_data_source->SetRootNode(root_node);

  // render manager bind uri loader
  render_manager_->SetUriLoader(context_->GetUriLoader());

  // create js driver
  driver_->SetConfig(config);
  driver_->Initialize(context_, dom_manager_, root_node, devtools_id);

  // 注册 console log 模块
  if (console_adaptor_) {
    footstone::log::LogMessage::InitializeDelegate(
        [console_adaptor = console_adaptor_](const std::ostringstream& stream, footstone::log::LogSeverity severity) {
          std::string str = stream.str();
          if (severity == footstone::log::LogSeverity::TDF_LOG_INFO) {
            console_adaptor->Info(str);
          } else if (severity == footstone::log::LogSeverity::TDF_LOG_WARNING) {
            console_adaptor->Warn(str);
          } else if (severity == footstone::log::LogSeverity::TDF_LOG_ERROR) {
            console_adaptor->Error(str);
          } else if (severity == footstone::log::LogSeverity::TDF_LOG_FATAL) {
            console_adaptor->Fatal(str);
          }
        });
  }

  // run script file
  bool ret = RunScriptFromUri("asset:/vendor.android.js");
  ret = RunScriptFromUri("asset:/index.android.js");

  return true;
}

void FrameWork::CreateDomManager() {
  dom_manager_ = std::make_shared<hippy::DomManager>();
  auto worker = std::make_shared<footstone::WorkerImpl>(kDomWorkerName, false);
  worker->Start();
  auto runner = std::make_shared<footstone::TaskRunner>(kDomRunnerName);
  runner->SetWorker(worker);
  worker->Bind({runner});
  dom_manager_->SetTaskRunner(runner);
  dom_manager_->SetWorker(worker);
}

uint32_t FrameWork::CreateDevtools() {
  std::string ws_url = "ws://127.0.0.1:38989/debugger-proxy?role=android_client&clientId=%s&hash=%s&contextName=%s";
  auto worker_manager = std::make_shared<footstone::WorkerManager>(kPoolSize);
  auto devtools_data_source = std::make_shared<hippy::devtools::DevtoolsDataSource>(ws_url, worker_manager);
  uint32_t id = devtools::DevtoolsDataSource::Insert(devtools_data_source);
  return id;
}

void FrameWork::CreateRenderManager() { render_manager_ = std::make_shared<hippy::render::tdf::TDFRenderManager>(); }

void FrameWork::CreateDriver() { driver_ = std::make_shared<hippy::Driver>(); }

bool FrameWork::RunScriptFromUri(footstone::stringview::StringViewUtils::string_view uri) {
  auto uri_loader = context_->GetUriLoader();
  auto ret = driver_->RunScriptFromUri(uri, uri_loader);
  return ret;
}

void FrameWork::LoadInstance() {
  uint32_t root_id = context_->GetRootNodeId();
  driver_->LoadInstance(root_id);
}

}  // namespace windows
}  // namespace framework
}  // namespace hippy
