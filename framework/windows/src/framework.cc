#include "framework.h"

#include <atomic>

#include "adaptor/console/event_viewer_console.h"
#include "footstone/deserializer.h"
#include "footstone/macros.h"

std::atomic<uint32_t> global_root_id{10};

namespace hippy {
inline namespace windows {
inline namespace framework {

static std::string CreateLoadInstanceMessage(uint32_t root_id) {
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

void Framework::Initialize(std::shared_ptr<Config>& config) {
  auto scope_callback = [WEAK_THIS](void*) {
    DEFINE_AND_CHECK_SELF(Framework)
    auto config = self->engine_->GetConfig();
    if (config) {
      std::string load_instance_message = CreateLoadInstanceMessage(config->GetRootId());
      if (self->engine_) {
        if (config->GetDebug()->GetDevelopmentModule()) {
          auto core_js = config->GetJsAssetsPath()->GetCorePath();
          self->engine_->RunScriptFromUri(string_view(core_js));
          self->engine_->RunScriptFromUri("asset:/index.android.js");
          self->engine_->LoadInstance(load_instance_message);
        } else {
          self->engine_->RunScriptFromUri("asset:/index.bundle");
          self->engine_->LoadInstance(load_instance_message);
        }
      }
    }
  };

  engine_ = std::make_shared<hippy::windows::framework::Engine>(config);
  engine_->Initialize();
  engine_->SetScopeCallback(scope_callback);
  return;
}

std::shared_ptr<hippy::Config> Framework::CreateDefaultConfig() {
  auto root_id = global_root_id.fetch_add(1);
  std::shared_ptr<hippy::Config> config = std::make_shared<hippy::Config>();
  config->GetJsEngine()->SetEnableV8Serialization(true);
  config->GetJsEngine()->SetUseCodeCache(true);
  config->GetJsEngine()->SetGroupId(-1);

  config->GetDebug()->SetDevelopmentModule(false);
  config->GetDebug()->SetHost("localhost:38989");
  config->GetJsAssetsPath()->SetCorePath("vendor.android.js");

  auto console_adaptor = std::make_shared<hippy::EventViewerConsole>("demo", true);
  config->GetAdaptor()->SetConsoleAdaptor(console_adaptor);

  config->SetDensity(1.0f);
  config->SetRootId(root_id);
  return config;
}

}  // namespace framework
}  // namespace windows
}  // namespace hippy
