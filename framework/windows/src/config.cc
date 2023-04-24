#include "config.h"

#include "footstone/hippy_value.h"
#include "footstone/serializer.h"

namespace hippy {
inline namespace framework {
inline namespace windows {
inline namespace config {

Config::Config() {}

Config::Config(bool enabel_v8_serialization, bool dev_module, bool use_code_cache, int64_t group_id,
               uint32_t devtools_id, string_view code_cache_directory, string_view global_config)
    : enabel_v8_serialization_(enabel_v8_serialization),
      dev_module_(dev_module),
      use_code_cache_(use_code_cache),
      group_id_(group_id),
      devtools_id_(devtools_id),
      code_cache_directory_(code_cache_directory),
      global_config_(global_config) {}

std::string Config::LoadInstanceString(uint32_t root_id) {
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

}  // namespace config
}  // namespace windows
}  // namespace framework
}  // namespace hippy
