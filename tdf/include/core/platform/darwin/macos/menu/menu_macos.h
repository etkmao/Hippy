//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#import <Cocoa/Cocoa.h>
#include <vector>
#include "core/platform/common/menu/menu.h"

@class TDFMenuItemTarget;

namespace tdfcore {

class MenuMacOS final : public Menu {
 public:
  void SetMenuBar(const std::shared_ptr<MenuList> &menu_list) override;
  void ShowMenuBar() override;
  void HideMenuBar() override;
  void PopupMenu(const std::shared_ptr<MenuList> &menu_list) override;
 private:
  NSMenu* CreateMenu(const std::vector<std::shared_ptr<MenuItemModel>>& menu_list);
  void SetupSubmenu(const std::shared_ptr<MenuItemModel> &menu_item_model, NSMenuItem *parent_item);
  NSMenuItem * MakeMenuItem(const std::shared_ptr<MenuItemModel> &menu_item_model);
  std::vector<TDFMenuItemTarget *> menu_bar_item_target_vector_;
  std::vector<TDFMenuItemTarget *> pop_menu_item_target_vector_;
  bool create_menu_from_menu_bar_ = false;
};

}  // namespace tdfcore
