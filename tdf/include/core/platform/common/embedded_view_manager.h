//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <unordered_map>
#include <vector>
#include "core/engine/schedule/task_runner.h"
#include "core/platform/common/embedded_view_holder.h"
#include "core/platform/common/output/embedded_delegate.h"
#include "core/platform/common/platform.h"

namespace tdfcore {
struct EmbeddedInfo;

typedef std::unordered_map<int64_t, std::shared_ptr<EmbeddedViewHolder>> EmbeddedViewHolderMap;

class EmbeddedViewManager : public Object,
                            public EmbeddedDelegate,
                            public std::enable_shared_from_this<EmbeddedViewManager> {
 public:
  ~EmbeddedViewManager() override = default;
  explicit EmbeddedViewManager(const std::shared_ptr<Platform> &platform,
                               const std::shared_ptr<PlatformView> &platform_view);

  /**
   * @brief 返回所有的EmbeddedInfo
   * @return
   */
  std::vector<EmbeddedInfo> EmbeddedInfos() override;

  /**
   * @brief 根据id返回对应的PlatformEmbeddedView
   * @param id
   * @return
   */
  std::shared_ptr<PlatformEmbeddedView> GetEmbeddedView(int64_t id);

  /**
   * @brief 返回EmbeddedViewHolder map信息
   * @return
   */
  std::unordered_map<int64_t, std::shared_ptr<EmbeddedViewHolder>> GetEmbeddedViewsMap() { return embedded_view_map_; }

  /**
   * @brief 通过embedded_view_infos更新embedded_view属性和层级信息
   * @param embedded_view_infos
   */
  std::future<void> UpdateEmbeddedInfo(const std::vector<EmbeddedInfo> &embedded_infos) override;

  void UpdateMutatorsStacks(const std::unordered_map<int64_t, MutatorsStack> &mutators_stacks_map) override;

  std::future<std::shared_ptr<Bitmap>> MakeSnapshot(int64_t id) override;

  /**
   * @brief 设置`更新MutatorsStacks完成后`的回调
   * @param callback 完成回调
   */
  void SetUpdateMutatorsStacksCallback(const std::function<void()> &callback);

 private:
  void UpdateEmbeddedInfoImpl(const std::vector<EmbeddedInfo> &embedded_infos);

  /**
   * @brief 生成新的id->EmbeddedViewHolder映射，EmbeddedViewHolder是可以复用的
   */
  EmbeddedViewHolderMap GenerateNewViewMap(const std::vector<EmbeddedInfo> &new_view_infos);

  std::shared_ptr<Platform> platform_;
  std::shared_ptr<PlatformView> platform_view_;
  std::shared_ptr<Scheduler> scheduler_;
  std::vector<EmbeddedInfo> embedded_infos_;
  EmbeddedViewHolderMap embedded_view_map_;
  std::function<void()> update_mutators_stacks_callback_;
};

}  // namespace tdfcore
