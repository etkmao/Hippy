//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include <vector>
#include "core/common/point.h"
#include "core/platform/common/channel/channel.h"

namespace tdfcore {

class MenuList;

/***
 * @brief 系统菜单基类，提供配置菜单条和弹出菜单功能
 */
class Menu : public Object {
 public:
  /***
   * @brief 设置菜单条
   * @param menu_list 菜单列表
   */
  virtual void SetMenuBar(const std::shared_ptr<MenuList> &menu_list) = 0;
  /***
   * @brief 显示菜单条
   */
  virtual void ShowMenuBar() = 0;
  /***
   * @brief 隐藏菜单条
   */
  virtual void HideMenuBar() = 0;
  /***
   * @brief 弹出菜单
   * @param menu_list 菜单列表
   * @param position 弹出位置
   */
  virtual void PopupMenu(const std::shared_ptr<MenuList> &menu_list) = 0;
};

/**
 * @brief 菜单项，一个菜单项可以包含一个菜单列表MenuList
 */
class MenuItemModel : public Object, public std::enable_shared_from_this<MenuItemModel> {
 public:
  using Action = std::function<void(std::shared_ptr<MenuItemModel> menu_item_model)>;

  explicit MenuItemModel(const std::string &title) : title_(title) {}
  ~MenuItemModel() override = default;

  /**
   * @brief 获取标题
   */
  std::string &GetTitle() { return title_; }

  /**
   * @brief 设置点击菜单项的action。注意调用的线程由平台系统确定。
   */
  void SetAction(const Action &action) { action_ = action; }

  /**
   * @brief 执行菜单项的action。注意调用的线程由平台系统确定。
   */
  void ExecuteAction() {
    if (action_) {
      action_(shared_from_this());
    }
  }

  /**
   * @brief 设置标识符
   */
  void SetTag(const int64_t tag) { tag_ = tag; }

  /**
   * @brief 获取标识符
   */
  int64_t GetTag() const { return tag_; }

  /**
   * @brief 设置是否可以选择
   */
  void SetEnabled(const bool enabled) { enabled_ = enabled; }

  /**
   * @brief 是否可以选择
   */
  bool IsEnabled() const { return enabled_; }

  /**
   * @brief 设置是否分隔线item
   */
  void SetSeparator(const bool separator) { separator_ = separator; }

  /**
   * @brief 是否分隔线item
   */
  bool IsSeparator() const { return separator_; }

  /**
   * @brief 设置菜单列表
   */
  void SetMenuList(const std::shared_ptr<MenuList> &menu_list) { menu_list_ = menu_list; }

  /**
   * @brief 获取菜单列表
   */
  std::shared_ptr<MenuList> &GetMenuList() { return menu_list_; }

 private:
  std::string title_;
  Action action_;
  int64_t tag_ = 0;
  bool enabled_ = true;
  bool separator_ = false;
  std::shared_ptr<MenuList> menu_list_;
};

/**
 * @brief 菜单列表MenuList，一个列表可以包含多个菜单项MenuItem。
 */
class MenuList : public Object {
 public:
  MenuList() = default;
  ~MenuList() override = default;

  /**
   * @brief 新增一个菜单项
   */
  void AddMenuItemModel(const std::shared_ptr<MenuItemModel> &item_model) {
    menu_item_model_vector_.push_back(item_model);
  }

  /**
   * @brief 获取所有菜单项
   */
  std::vector<std::shared_ptr<MenuItemModel>> GetMenuItemModels() { return menu_item_model_vector_; }

 private:
  std::vector<std::shared_ptr<MenuItemModel>> menu_item_model_vector_;
};

}  // namespace tdfcore
