//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <future>
#include <map>
#include <string>
#include <vector>
#include "core/common/rect.h"
#include "core/engine/graphics/texture.h"
#include "core/common/bitmap.h"
#include "core/platform/common/view/mutators_stack.h"
#include "core/platform/windows/view/tdf_view.h"
#include "core/platform/windows/windows_define.h"

namespace tdfcore {

/**
 * @brief 原生控件View，使用时需要子类化
 */
class EmbeddedHostView : public TDFView {
 public:
  EmbeddedHostView() = default;
  ~EmbeddedHostView() = default;

  /**
   * @brief 因为TDFEmbeddedFactoryWin不支持带参数构建，这里单独给一个方法传参。自定义子类需要优先调用父类此方法
   */
  virtual void setupProperties(const std::map<std::string, std::string> properties);

  /**
   * @brief 立即更新变换栈
   */
  void UpdateMutators(const MutatorsStack& mutators_stack);

  /**
   * @brief 更新View的属性
   * @param properties view的属性
   */
  void UpdateProps(const std::map<std::string, std::string>& properties);

  /**
   * @brief 从视图树里面移除调用，释放相关资源
   */
  void OnDispose();

  /**
   * @brief 返回当前截图Bitmap
   * @return
   */
  std::future<std::shared_ptr<Bitmap>> MakeSnapshot();

  /**
   * @brief 窗口消息处理
   */
  virtual LRESULT MessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;
};

}  // namespace tdfcore
