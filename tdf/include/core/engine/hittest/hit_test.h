//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "core/common/object.h"
#include "core/common/point.h"

namespace tdfcore {
class HitTestResult;
class RenderObject;

/**
 * 决定 RenderObject HitTest 的表现行为
 */
enum class HitTestBehavior {
  // 不透明，自己需要响应 HitTest, 且被自己遮住的兄弟节点都无法响应 HitTest
  kOpaque,
  // 半透明，自己需要响应 HitTest, 且被自己遮住的兄弟节点可以响应 HitTest
  kTranslucent,
  // 忽略，自己不响应 HitTest 及其自己的子节点也无法响应，但是兄弟节点可以响应 HitTest
  kIgnore,
};

class HitTestResult : public Object {
 public:
  ~HitTestResult() override = default;
  constexpr const std::vector<std::shared_ptr<RenderObject>> &GetPath() const { return path_; }

  /**
   * 两个 HitTestResult 结果拼凑起来
   * @param other 需要拼凑的目标
   * @return 合并结果
   */
  void Merge(const std::shared_ptr<HitTestResult> &other);

  /**
   *
   * @return 返回失败的 HitTest 结果
   */
  static std::shared_ptr<HitTestResult> Fail();

  /**
   * 检测是否有效的 HitTest 结果，如果 HitTestResult 没有添加过 render object, 则是无效的
   * @return 检测是否有效的 HitTest 结果
   */
  bool IsValid() const { return !path_.empty(); }

  /**
   * @brief 添加一个新的entry到path_的末尾。
   *
   * @param entry
   */
  void AddEntry(const std::shared_ptr<RenderObject> &entry);

 protected:
  HitTestResult() = default;

 private:
  bool IsContainEntry(std::shared_ptr<RenderObject> entry);

  std::vector<std::shared_ptr<RenderObject>> path_;

  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
