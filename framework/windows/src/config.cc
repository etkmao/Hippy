#include "config.h"

namespace hippy {
inline namespace windows {
inline namespace framework {

Config::Config()
    : js_assets_path_(std::make_shared<Config::JsAssetsPath>()),
      js_engine_(std::make_shared<Config::JsEngine>()),
      debug_(std::make_shared<Config::Debug>()) {}

}  // namespace framework
}  // namespace windows
}  // namespace hippy
