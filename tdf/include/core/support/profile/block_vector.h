//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <vector>
#include <cstring>
#include "core/common/object.h"

namespace tdfcore {

/**
 * @brief 以Block为单位进行循环复用的容器
 * @note 此为通用数据结构，但更适用于Profile模块的需求场景，暂时不开放给外部使用
 * @detail
 *   【Block的概念】
 *   它代表一个一次性分配出来的大块内存，它可以存储capacity个数据项
 *
 *   【Block的管理】
 *   （1）本容器以Block为单位，新增存储区域，或复用存储区域
 *   （2）本容器维护两个Block数组，used_blocks与recycled_blocks
 *   （3）used_blocks管理正在存储数据项的所有Block
 *   （4）recycled_blocks管理所有已经被淘汰的可以拿来复用的空Block
 *
 *   【存储 & 扩容】
 *   （1）调用者通过容器的PushBack方法，存入新数据项，新数据项将被写入used_blocks尾部的那个Block中
 *   （2）当used_blocks尾部的那个Block写满时，容器会新增or复用一个Block，该Block成为used_blocks新的尾部
 *
 *   【淘汰 & 复用】
 *   （1）调用者通过容器的RecycleBlocks方法，可以检查当前used_blocks中所有的Block，并决定淘汰掉哪些Block
 *   （2）被淘汰的Block，将被加入recycled_blocks数组中（数量有上限），等待容器在扩容时被拿来复用
 */
template <typename T>
struct BlockVector {
 public:
  /**
   * @brief 独立的内存块
   */
  struct Block {
    // 能够存储的数据项的总容量
    size_t capacity = 0;
    // 实际存储数据项的总数
    size_t size = 0;
    // 存储数据项的内存区域
    T *items = nullptr;

    /**
     * @brief 构造函数
     * @param capacity 能够存储的数据项的总容量
     */
    explicit Block(size_t capacity) : capacity(capacity) {}

    /**
     * @brief 写入一个数据项
     * @param item 数据项的指针
     */
    inline void PushBack(T *item) {
      memcpy(reinterpret_cast<void *>(items + size), reinterpret_cast<const void *>(item), sizeof(T));
      size++;
    }

    /**
     * @brief 找到第一个满足匹配条件的数据项的索引
     * @param matcher 匹配器，返回true表示满足条件
     */
    inline size_t FindFirstIndex(const std::function<bool(const T *item)> &matcher) const {
      if ((size == 0) || ((size > 0) && !matcher(items + size - 1))) {
        return size;
      }
      size_t low = 0;
      size_t high = size - 1;
      while (low < high) {
        size_t middle = (low + high) >> 1;
        if (matcher(items + middle)) {
          high = middle;
        } else {
          low = middle + 1;
        }
      }
      return (matcher(items + high)) ? high : size;
    }
  };

  /**
   * @brief 构造函数
   * @param capacity_per_block 单个Block的容量
   */
  explicit BlockVector(size_t capacity_per_block = kDefaultCapacityPerBlock) : capacity_per_block(capacity_per_block) {}

  BlockVector(const BlockVector &) = delete;
  BlockVector &operator=(const BlockVector &) = delete;

  virtual ~BlockVector() {
    for (auto &block : used_blocks) {
      free(block.items);
    }
    for (auto &block : recycled_blocks) {
      free(block.items);
    }
  }

  /**
   * @brief 写入一个数据项
   * @param item 数据项的指针
   */
  void PushBack(T *item) {
    if (used_blocks.empty() || (used_blocks.back().size >= capacity_per_block)) {
      if (recycled_blocks.empty()) {
        Block block(capacity_per_block);
        block.items = reinterpret_cast<T *>(malloc(sizeof(T) * capacity_per_block));
        used_blocks.push_back(block);
      } else {
        used_blocks.push_back(recycled_blocks.back());
        recycled_blocks.pop_back();
      }
    }
    used_blocks.back().PushBack(item);
  }

  /**
   * @brief 获取所有存储数据项的Block
   */
  constexpr const std::vector<Block> &GetBlocks() const { return used_blocks; }

  /**
   * @brief 淘汰回收Block
   * @param checker 检测器，返回true表示该Block需要被淘汰回收
   */
  void RecycleBlocks(const std::function<bool(const Block &)> &checker) {
    assert(checker);
    auto iter = used_blocks.begin();
    while (iter != used_blocks.end()) {
      if (checker(*iter)) {
        if (recycled_blocks.size() < kDefaultRecycleLimit) {
          iter->size = 0;
          recycled_blocks.push_back(*iter);
        } else {
          free(iter->items);
        }
        iter = used_blocks.erase(iter);
      } else {
        iter++;
      }
    }
  }

 private:
  static const size_t kDefaultCapacityPerBlock = 512;
  static const size_t kDefaultRecycleLimit = 1;
  size_t capacity_per_block;
  std::vector<Block> used_blocks;
  std::vector<Block> recycled_blocks;
};

}  // namespace tdfcore
