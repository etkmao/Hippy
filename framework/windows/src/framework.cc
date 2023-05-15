#include "framework.h"

#include <atomic>

#include "adaptor/console/event_viewer_console.h"
#include "footstone/macros.h"

std::atomic<uint32_t> global_root_id{10};

namespace hippy {
inline namespace windows {
inline namespace framework {

void Framework::Initialize(std::shared_ptr<Config>& config) {
  engine_ = std::make_shared<hippy::windows::framework::Engine>(config);
  engine_->Initialize();
  engine_->SetScopeInitializedCallback([WEAK_THIS, config](uint32_t root_id) {
    DEFINE_AND_CHECK_SELF(Framework)
    std::string load_instance_message = Engine::CreateLoadInstanceMessage(root_id);
    if (config->GetDebug()->GetDevelopmentModule()) {
      std::string remote_uri = Engine::CreateRemoteUri(config);
      self->engine_->RunScriptFromUri(string_view(remote_uri));
      self->engine_->LoadInstance(load_instance_message);
    } else {
      auto core_js = config->GetJsAssetsPath()->GetCorePath();
      self->engine_->RunScriptFromUri(string_view(core_js));
      self->engine_->RunScriptFromUri("asset:/index.android.js");
      self->engine_->LoadInstance(load_instance_message);
    }
  });
  return;
}

void Framework::Reload() {
  if (engine_) {
    auto new_root_id = global_root_id.fetch_add(1);
    engine_->ReloadInstance(new_root_id);
  }
}

std::shared_ptr<hippy::Config> Framework::CreateDefaultConfig() {
  auto root_id = global_root_id.fetch_add(1);
  std::shared_ptr<hippy::Config> config = std::make_shared<hippy::Config>();
  config->GetJsEngine()->SetEnableV8Serialization(true);
  config->GetJsEngine()->SetUseCodeCache(true);
  config->GetJsEngine()->SetGroupId(-1);

  // config->GetDebug()->SetDevelopmentModule(false);
  config->GetDebug()->SetDevelopmentModule(true);
  config->GetDebug()->SetHost("localhost:38989");
  config->GetDebug()->SetBundleName("index.bundle");

  config->GetJsAssetsPath()->SetCorePath("vendor.android.js");

  auto console_adaptor = std::make_shared<hippy::EventViewerConsole>("demo", true);
  console_adaptor->Initial();
  config->GetAdaptor()->SetConsoleAdaptor(console_adaptor);

  config->SetDensity(1.0f);
  config->SetRootId(root_id);
  return config;
}

}  // namespace framework
}  // namespace windows
}  // namespace hippy
