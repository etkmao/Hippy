//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <utility>
#include "tdfui/view/view.h"
namespace tdfcore {
/**
 * @brief  转场动画上下文
 */
class TransitionContext : public Object {
 public:
  ~TransitionContext() override = default;
  const std::shared_ptr<View>& GetFromView() const { return from_view_; }

  const std::shared_ptr<View>& GetToView() const { return to_view_; }

  const std::shared_ptr<View>& GetContainerView() { return container_view_; }

  bool IsPush() const { return is_push_; }

  bool IsPop() const { return !is_push_; }

  std::function<void()> GetCompleteHandler() { return complete_handler_; }

  std::function<void()> GetCancelHandler() { return cancel_handler_; }

  void Finalizer();

 protected:
  TransitionContext(std::shared_ptr<View> from_view, std::shared_ptr<View> to_view,
                    std::shared_ptr<View> container_view, bool is_push, std::function<void()> complete_handler,
                    std::function<void()> cancel_handler);

 private:
  std::shared_ptr<View> from_view_;
  std::shared_ptr<View> to_view_;
  std::shared_ptr<View> container_view_;
  bool is_push_ = true;
  std::function<void()> complete_handler_;
  std::function<void()> cancel_handler_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore
