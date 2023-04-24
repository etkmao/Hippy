//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <vector>
#include "core/common/macro.h"

namespace tdfcore {

/**
 * @brief 基于散列的无序Map
 * @param K 键的类型
 * @param V 值的类型
 * @param H 散列算法的类型
 * @note 此为通用数据结构，但更适用于Profile模块的需求场景，暂时不开放给外部使用
 * @detail
 *   【散列算法】
 *   散列算法需要通过模板H给出，调用者需要自定义一个如下的类型，将该类型作为模板H即可：
 *   struct XXXHash {
 *     size_t operator()(const K &key) const { return XXX; }
 *   };
 *   本文件尾部的PointerHash就是一个可供参考的样例
 *
 *   【折中 & 定制】
 *   相比于std::unordered_map，实现上有折中，同时也有定制需求：
 *   （1）突出了Insert与Erase操作的性能，且定制了这两个操作的附加能力
 *   （2）牺牲了迭代器相关的能力
 *
 *   【数据结构之间的关系】
 *   （1）Pair表示一个由key和value组成的键值对
 *   （2）Table是散列表对象，它维护了一个存储散列用的内存块buckets，其实际类型为Bucket[]
 *   （2）Bucket代表散列表中的一个槽位，由于散列会有冲突，所以，凡是散列到同一个槽位的Pair，都会存入同一个Bucket
 *   （3）Bucket内部以链表的方式组织所有的Pair，故Pair都是包在一个ListNode中的
 *
 *   【存储 & 复用】
 *   （1）容器提供了一个Pair的缓存池PairPool
 *   （2）新插入键值对时，会从PairPool中获取一个空Pair，待数据填入后，该Pair被散列至某一个Bucket中
 *   （3）当某个键值对被移除时，它对应的Pair会被归还入PairPool
 *   （4）PairPool的每一次扩容，都以一个固定容量（大小等于散列表的容量），一次性分配好空间，只扩容不缩容
 */
template <typename K, typename V, typename H>
struct HashMap {
 public:
  /**
   * @brief 构造函数
   * @param bucket_count 散列表的容量
   */
  explicit HashMap(size_t bucket_count = kDefaultBucketCount) {
    pool = new PairPool(bucket_count);
    table = new Table(bucket_count);
  }

  ~HashMap() {
    delete table;
    delete pool;
  }

  /**
   * @brief 当前Map中键值对的总数
   */
  constexpr size_t Size() const { return size; }

  /**
   * @brief 获取键key所对应的值
   */
  V *GetValue(const K &key) const {
    auto pair = table->GetBucket(Hash(key))->Find(key);
    return pair ? &(pair->data.value) : nullptr;
  }

  /**
   * @brief 插入键值对
   * @param key 键
   * @param value 值
   * @param over_write 当Map中已经存在该key的键值对时，是否用新的键值对进行覆盖
   */
  bool Insert(const K &key, const V &value, bool over_write = true) {
    auto bucket = table->GetBucket(Hash(key));
    auto pair = bucket->Find(key);
    if (!pair) {
      pair = pool->Fetch();
      bucket->Insert(pair);
      size++;
    } else if (!over_write) {
      return false;
    }
    pair->data.Reset(key, value);
    return true;
  }

  /**
   * @brief 移除指定的键值对
   * @result 键值对存在，并进行移除操作，则返回true；键值对不存在，无法进行移除操作，则返回false
   */
  bool Erase(const K &key) {
    Bucket *bucket = table->GetBucket(Hash(key));
    auto pair = bucket->Find(key);
    if (pair) {
      bucket->Erase(pair);
      pool->Reclaim(pair);
      assert(size > 0);
      size--;
      return true;
    }
    return false;
  }

  /**
   * @brief 遍历所有的键值对
   * @param callback 回调处理，每一个键值对回调一次
   */
  void Enumerate(const std::function<void(const K &key, const V &value)> &callback) const {
    for (size_t i = 0; i < table->capacity; ++i) {
      table->GetBucket(i)->Enumerate(callback);
    }
  }

 protected:
  template <typename T>
  struct ListNode {
    ListNode *next = nullptr;
    ListNode *prev = nullptr;
    T data;

    inline void AddToList(ListNode **header) {
      prev = nullptr;
      next = *header;
      if (*header) {
        (*header)->prev = this;
      }
      *header = this;
    }

    inline void RemoveFromList(ListNode **header) {
      if (*header == this) {
        *header = (*header)->next;
      }
      if (prev) {
        prev->next = next;
      }
      if (next) {
        next->prev = prev;
      }
      prev = nullptr;
      next = nullptr;
    }
  };

  struct Pair {
    K key;
    V value;

    inline void Reset(const K &k, const V &v) {
      memcpy(reinterpret_cast<void *>(&key), reinterpret_cast<const void *>(&k), sizeof(V));
      memcpy(reinterpret_cast<void *>(&value), reinterpret_cast<const void *>(&v), sizeof(V));
    }
  };

  struct PairPool {
    size_t capacity_per_block = 0;
    ListNode<Pair> *pair_list = nullptr;
    std::vector<ListNode<Pair> *> blocks;

    explicit PairPool(size_t capacity_per_block) : capacity_per_block(capacity_per_block) {}

    ~PairPool() {
      for (const auto &block : blocks) {
        delete[] block;
      }
    }

    inline ListNode<Pair> *Fetch() {
      if (!pair_list) {
        Expand();
      }
      auto pair = pair_list;
      pair->RemoveFromList(&pair_list);
      return pair;
    }

    inline void Reclaim(ListNode<Pair> *pair) { pair->AddToList(&pair_list); }

    inline void Expand() {
      auto *pairs = new ListNode<Pair>[capacity_per_block];
      // 将块内的所有所Pair用链表串联在一起
      for (size_t i = 0; i < capacity_per_block - 1; ++i) {
        pairs[i].next = pairs + i + 1;
      }
      for (size_t i = 1; i < capacity_per_block; ++i) {
        pairs[i].prev = pairs + i - 1;
      }
      pair_list = pairs;
      blocks.push_back(pairs);
    }
  };

  struct Bucket {
    ListNode<Pair> *pair_list = nullptr;

    inline ListNode<Pair> *Find(const K &key) {
      auto pair = pair_list;
      while (pair) {
        if (pair->data.key == key) {
          return pair;
        }
        pair = pair->next;
      }
      return nullptr;
    }

    inline void Insert(ListNode<Pair> *pair) { pair->AddToList(&pair_list); }

    inline void Erase(ListNode<Pair> *pair) { pair->RemoveFromList(&pair_list); }

    inline void Enumerate(const std::function<void(const K &key, const V &value)> &callback) {
      auto pair = pair_list;
      while (pair) {
        callback(pair->data.key, pair->data.value);
        pair = pair->next;
      }
    }
  };

  struct Table {
    size_t capacity = 0;
    Bucket *buckets = nullptr;

    explicit Table(size_t capacity) : capacity(capacity) { buckets = new Bucket[capacity]; }

    ~Table() { delete[] buckets; }

    inline Bucket *GetBucket(size_t index) { return buckets + index; }
  };

 private:
  static const size_t kDefaultBucketCount = 8192;
  Table *table = nullptr;
  PairPool *pool = nullptr;
  H hash_generator;
  size_t size = 0;

  inline size_t Hash(const K &key) { return hash_generator(key) % table->capacity; }
};

/**
 * @brief 指针散列算法
 * @detail 本类会将int64_t类型的Key视为指针，根据指针自身的特点做散列算法。此类配合tdfcore::HashMap一起使用。
 *   【算法思路】
 *   （1）指针在不做任何处理的情况下，本身其实已经变现出比较好的散列性，故不需要进行复杂的散列算法
 *   （2）取指针中变化最大的区域，用做散列算法的素材，效果最佳
 *   （3）64位地址变化最大的区域仍旧是低32位区域，另外地址的低3位永远是0，故散列算法实际考虑的取材范围是低3~31位
 *   （4）将低3~31位再次划分成高与低两个部分，做一次异或，这个结果就是最终的散列值
 *   （5）算法中可变的参数常量是20，它是对低3~31位的分割线，目前来看20的效果比较好
 */
struct PointerHash {
  size_t operator()(const int64_t &key) const { return (key >> 20) ^ (key >> 3); }
};

}  // namespace tdfcore
