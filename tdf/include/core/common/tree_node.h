//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include <cassert>
#include <any>
#include <functional>
#include <map>
#include <memory>
#include "core/common/macro.h"
#include "core/common/object.h"
#include "core/common/reflection.h"
#include "core/common/diagnostics_reflection.h"

namespace tdfcore {

template<typename ObjectType, typename ContextType>
class TreeNode : public Object,
                 public Reflectable,
                 public Diagnosticable,
                 public std::enable_shared_from_this<TreeNode<ObjectType, ContextType>> {
TDF_REFF_CLASS_META(TreeNode)

 public:
  ~TreeNode() override = default;

  virtual void Detach() {
    assert(!context_.expired());
    assert_fn([&]() {
      if (auto parent = parent_.lock()) {
        return parent != nullptr && parent->GetContext() == nullptr;
      }
      return true;
    });
    context_.reset();
  }

  virtual void Attach(std::shared_ptr<ContextType> context) {
    assert(context != nullptr);
    context_ = std::weak_ptr<ContextType>(context);
  }

  inline bool IsAttached() const { return !context_.expired(); }

  constexpr std::shared_ptr<ContextType> GetContext() const { return context_.lock(); }

  virtual void AdoptChild(std::shared_ptr<TreeNode> child) {
    assert(child != nullptr);
    assert(child.get() != this);
    assert(child->parent_.expired());
    assert_fn([&]() {
      // 检测是否有循环
      auto node = this;
      auto parent = node->parent_.lock();
      while (parent != nullptr) {
        if (parent == child) {
          return false;
        }
        node = parent.get();
        parent = node->parent_.lock();
      }
      return true;
    });
    child->parent_ = this->shared_from_this();
    if (IsAttached()) {
      child->Attach(context_.lock());
    }
  }

  virtual void DropChild(std::shared_ptr<TreeNode> child) {
    assert(child != nullptr);
    assert_fn([&]() {
      auto parent = child->parent_.lock();
      return (parent == nullptr) || (parent.get() == this);
    });
    child->parent_.reset();
    if (IsAttached()) {
      child->Detach();
    }
  }

  virtual void VisitChildren(const std::function<void(std::shared_ptr<ObjectType> &child)> &visitor) {}

  /**
   *
   * @param key 配置在 object_info_ 的 关键字
   * @return key 对应的数据
   */
  std::optional<std::any> GetExtraField(const std::string &key) const {
    auto found = extra_fields_.find(key);
    if (found == extra_fields_.end()) {
      return {};
    }
    return found->second;
  }

  /**
   *
   * @param key 需要设置的关键字
   * @param value 关键字返回的数据
   */
  void SetExtraField(const std::string &key, const std::any &value) { extra_fields_[key] = value; }

  std::shared_ptr<Diagnostics> ToDiagnostics() const override {
    return MakeDiagnosticsByReflection(this);
  }

 protected:
  std::weak_ptr<ContextType> context_;
  std::weak_ptr<TreeNode> parent_;
  // 配置 extra_fields_, 让子类及其自己可以自定义数据，让外部识别
  std::map<std::string, std::any> extra_fields_;
};

}  // namespace tdfcore

TDF_REFL_TEMPLATE_DEFINE((typename ObjectType, typename ContextType),
                         (tdfcore::TreeNode<ObjectType, ContextType>), bases<>)
TDF_REFL_END(tdfcore::TreeNode)
