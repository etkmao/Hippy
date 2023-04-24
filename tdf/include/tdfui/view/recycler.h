//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <unordered_map>
#include "tdfui/view/view.h"

namespace tdfcore {
/**
 * @brief 回收站
 */
class Recycler : public Object {
 public:
  /**
   * @brief 放置进回收池
   * @param view_type 视图类型
   * @param view 视图
   */
  virtual void AddView(int64_t view_type, const std::shared_ptr<View> &view) = 0;

  /**
   * @brief 从回收池中取
   * @param view_type 视图类型
   * @return 视图（若没有该类型的视图则返回空）
   */
  virtual std::shared_ptr<View> GetView(int64_t view_type) = 0;

  /**
   * @brief 清空回收站
   */
  virtual void Clear() = 0;
};

/**
 * @brief 默认回收站
 */
class DefaultRecycler : public Recycler {
 public:
  ~DefaultRecycler() override = default;
  void AddView(int64_t view_type, const std::shared_ptr<View> &view) override;
  std::shared_ptr<View> GetView(int64_t view_type) override;
  void Clear() override;

 private:
  std::unordered_map<int64_t, std::vector<std::shared_ptr<View>>> views_map_;
};
}  // namespace tdfcore
