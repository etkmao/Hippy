//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <future>
#include "core/common/bitmap.h"
#include "core/platform/common/view/mutators_stack.h"

namespace tdfcore {
struct EmbeddedInfo;
/**
 * @brief 外部更新EmbeddedInfo代理类
 */
class EmbeddedDelegate {
 public:
  /**
   * @brief
   * 在光栅化阶段会收集所有Embedded节点的信息，启动TextureOutput模式，可以通过实现此代理方法，把Embedded信息更新到外面
   * 每一帧由数据刷新都会调用此方法，上一帧被移除的Embedded不会出现在当前embedded_infos里面，外部可以根据此方法实现新增或者移除逻辑
   * @note 此调用在RasterTaskRunner里面执行
   * @param embedded_infos
   * @return 返回异步future，在截图场景下会同步等待EmbeddedInfo的更新完成
   */
  virtual std::future<void> UpdateEmbeddedInfo(const std::vector<EmbeddedInfo> &embedded_infos) = 0;

  virtual std::vector<EmbeddedInfo> EmbeddedInfos() = 0;

  /**
   * @brief 更新Embedded的累计变换信息，主要包含Transform、Clip等信息，每一帧都会刷新
   * @note
   * MutatorStack信息是按照RenderTree遍历顺序排列的，所有计算Transform和Clip信息是需要依次计算，可以参考Android/iOS实现
   * @note 此调用在RasterTaskRunner里面执行
   */
  virtual void UpdateMutatorsStacks(const std::unordered_map<int64_t, MutatorsStack> &mutators_stacks_map) = 0;

  /***
   * @brief 返回Embedded对应的截图，需要立即返回截图
   * @param id embedded对应的id
   * @return 返回异步future，截图会同步等待Embedded返回截图信息
   */
  virtual std::future<std::shared_ptr<Bitmap>> MakeSnapshot(int64_t id) = 0;
};

}  // namespace tdfcore
