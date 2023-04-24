//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once
#include <unordered_map>
#include <vector>
#include "core/platform/common/menu/menu.h"
#include "core/platform/windows/windows_define.h"

namespace tdfcore {

/**
 * @brief 顶部菜单栏
 */
class TaskRunner;
class MenuWindows final : public Menu {
 public:
  explicit MenuWindows(HWND window_handle);
  ~MenuWindows() = default;
  void SetMenuBar(const std::shared_ptr<MenuList> &menu_list) override;
  void ShowMenuBar() override;
  void HideMenuBar() override;
  void PopupMenu(const std::shared_ptr<MenuList> &menu_list) override;
  /**
   * @brief 根据command_id匹配菜单项执行其action
   * @return 成功找到匹配command_id的菜单项则返回true
   */
  bool ExecuteAction(const uint16_t command_id);

 private:
  void AppendMenuItem(HMENU parent_menu, const std::shared_ptr<MenuItemModel> &menu_item_model);
  HMENU MakeMenuAndSubmenu(const std::shared_ptr<MenuItemModel> &menu_item_model);
  HWND window_handle_;
  std::unordered_map<uint16_t, std::shared_ptr<MenuItemModel>> menu_bar_item_models_;
  std::unordered_map<uint16_t, std::shared_ptr<MenuItemModel>> pop_menu_item_models_;
  bool create_menu_from_menu_bar_ = false;
  HMENU current_menu_bar_ = NULL;
};

}  // namespace tdfcore
