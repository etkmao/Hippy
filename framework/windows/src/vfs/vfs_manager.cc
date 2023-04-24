#include "vfs/vfs_manager.h"

#include <atomic>

namespace hippy {
inline namespace framework {
inline namespace windows {

std::atomic<uint32_t> uri_loader_id{1};

uint32_t VfsManager::CreateUriLoader() {
  auto id = uri_loader_id.fetch_add(1);
  auto loader = std::make_shared<hippy::UriLoader>();
  uri_loader_map_.insert({id, loader});
  return id;
};

std::shared_ptr<hippy::vfs::UriLoader> VfsManager::GetUriLoader(uint32_t id) {
  auto loader = uri_loader_map_.find(id);
  if (loader == uri_loader_map_.end()) return nullptr;
  return loader->second;
}

}  // namespace windows
}  // namespace framework
}  // namespace hippy
