//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <list>
#include <unordered_map>
#include "core/common/object.h"
namespace tdfcore {
template <class K, class V>
struct Node {
  K key;
  V value;
  int64_t size;

  Node(K key, V value, int64_t size) : key(key), value(value), size(size) {}
};

template <class K, class V, class _Hash = std::hash<K>, class _Pred = std::equal_to<K>>
class LRUCache {
 public:
  explicit LRUCache(int64_t capacity) : capacity_(capacity) {}

  std::pair<V, int64_t> Get(const K &key) {
    if (map_.find(key) == map_.end()) {
      return std::make_pair(V(), 0);
    }
    // 移动到首位
    list_.splice(list_.begin(), list_, map_[key]);
    map_[key] = list_.begin();
    return std::make_pair(map_[key]->value, map_[key]->size);
  }

  void Set(const K &key, const V &value, int64_t size) {
    if (size > capacity_) {
      return;
    }
    total_size_ += size;
    auto map_iter = map_.find(key);
    if (map_iter == map_.end()) {
      // 插入第一个位置
      list_.push_front(Node(key, value, size));
      map_[key] = list_.begin();
    } else {
      // 更新值
      total_size_ -= map_iter->second->size;
      map_iter->second->size = size;
      map_iter->second->value = value;
      // 移动到首位
      list_.splice(list_.begin(), list_, map_iter->second);
      map_[key] = list_.begin();
    }
    // 检查大小，若超过限制 一直末位淘汰
    TrimToCapacity();
  }

  void Clear() {
    total_size_ = 0;
    list_.clear();
    map_.clear();
  }

  void SetCapacity(int64_t capacity) {
    capacity_ = capacity;
    TrimToCapacity();
  }

  bool Contains(const K &key) const { return map_.find(key) != map_.end(); }

  int64_t GetCapacity() const { return capacity_; }

  std::pair<V, int64_t> Remove(const K &key) {
    auto map_iter = map_.find(key);
    if (map_iter == map_.end()) {
      return std::make_pair(V(), 0);
    }
    auto pair = std::make_pair(map_iter->second->value, map_iter->second->size);
    total_size_ -= map_iter->second->size;
    list_.erase(map_iter->second);
    map_.erase(map_iter);
    return pair;
  }

  bool Empty() {
    return map_.empty();
  }

 private:
  void TrimToCapacity() {
    while (total_size_ > capacity_ && !list_.empty()) {
      auto map_iter = map_.find(list_.back().key);
      total_size_ -= map_iter->second->size;
      map_.erase(map_iter);
      list_.pop_back();
    }
  }
  int64_t capacity_ = 0;
  int64_t total_size_ = 0;
  std::list<Node<K, V>> list_;
  std::unordered_map<K, typename std::list<Node<K, V>>::iterator, _Hash, _Pred> map_;
};
}  // namespace tdfcore
