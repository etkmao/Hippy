#include "context.h"

#include <atomic>
#include <fstream>

#include "dom/root_node.h"

std::atomic<uint32_t> global_root_id{10};

namespace hippy {
inline namespace framework {
inline namespace windows {

void Context::CreateRootNode() {
  root_id_ = global_root_id.fetch_add(1);
  auto root_node = std::make_shared<hippy::RootNode>(root_id_);
  auto& persistent_map = hippy::RootNode::PersistentMap();
  auto flag = persistent_map.Insert(root_id_, root_node);
  FOOTSTONE_DCHECK(flag);
}

std::shared_ptr<RootNode> Context::GetRootNode() {
  std::shared_ptr<hippy::RootNode> root_node = nullptr;
  auto& persistent_map = hippy::RootNode::PersistentMap();
  persistent_map.Find(root_id_, root_node);
  return root_node;
}

void Context::CreateUriLoader() {
  FOOTSTONE_DCHECK(vfs_manager_ != nullptr);
  uri_loader_id_ = vfs_manager_->CreateUriLoader();
}

std::shared_ptr<hippy::UriLoader> Context::GetUriLoader() {
  if (vfs_manager_ == nullptr) return nullptr;
  return vfs_manager_->GetUriLoader(uri_loader_id_);
}

}  // namespace windows
}  // namespace framework
}  // namespace hippy
