//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <functional>
#include <memory>
#include <optional>
#include "core/common/value_change_notifier.h"
#include "core/platform/common/clipboard.h"

namespace tdfcore {

enum class ClipboardStatus { Pasteable, Unknown, NotPasteable };

class ClipboardStatusNotifier : public Object, public ValueNotifier<ClipboardStatus>,
                                public std::enable_shared_from_this<ClipboardStatusNotifier> {
 public:
  ~ClipboardStatusNotifier() override = default;

  /**
   * @brief 更新键盘状态
   */
  void Update();

  /**
   * @brief 添加回调
   * @param listener 回调
   * @return 添加的回调的下标
   */
  uint64_t AddListener(Callback listener) override;

  /**
   * @brief 移除回调
   * @param handler  addlistener返回的下标
   */
  void RemoveListener(uint64_t handler) override;

  /**
   * @brief 销毁时的调用
   */
  void Dispose() override;

  /**
   * @brief 返回是否销毁状态
   * @return
   */
  bool Disposed() const { return disposed_; }

 protected:
  explicit ClipboardStatusNotifier(ClipboardStatus value = ClipboardStatus::Unknown);

 private:
  std::optional<uint64_t> listener_id_;
  bool disposed_ = false;
  FRIEND_OF_TDF_ALLOC;
};

}  // namespace tdfcore
