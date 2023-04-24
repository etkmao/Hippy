//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/memory.h"
#include "tdfui/view/scroll_view_header.h"

namespace tdfcore {

enum class RefreshHeaderState {
  kIdle,              // 空闲状态
  kPulling,           // 下拉过程
  kReleaseToRefresh,  // 释放立即刷新
  kRefreshing,        // 正在刷新
  kCompleted          // 刷新完成
};

/**
 * 一个控制"下拉刷新控件状态"的自动机，之前直接在 RefreshHeader 修改状态有以下问题：
 *   1. 容易导致诸多边界Case没有考虑，导致Bug多
 *   2. SetState 和 UI逻辑耦合
 *   3. SetState 分散，排查问题困难，可读性也比较差
 *
 * 自动机也不是万能的，业务代码 *仍然需要自己判断当前状态，做出合适的状态转移*。
 *
 * 但是，使用自动机后有以下好处：
 *   1. 行为（UI）和状态分离，两者的错误不会耦合在一起
 *   2. 可读性提升，调试方便
 *   3. 可以在状态转移阶段就暴露错误，无需通过UI错误回溯
 *
 *
 *                   加载完成自动重置（框架逻辑）
 *  ──────────────Idle◄───────────────Completed
 *  │    开始正向滑动│ ▲                    ▲
 *  │              ▼ │滑回起始位置          │
 *  │           Pulling                   │
 *  │        滑过阈值│ ▲                   │ 调用方数据加载完成，主动调用（业务逻辑）
 *  │直接滚动到阈值   ▼ │滑回阈值以内          │
 *  ──────►ReleaseToRefresh────────►Refreshing
 *                  用户释放，触发刷新（用户行为，Pulling也是）
 */
class RefreshStateAutomata {
  /**
   * @brief 监听刷新状态的变化
   */
  using StateChangeCallback = std::function<void(RefreshHeaderState)>;

 public:
  virtual ~RefreshStateAutomata() = default;

  void TransferToIdle();

  void TransferToPulling();

  void TransferToReleaseToRefresh();

  void TransferToRefreshing();

  void TransferToCompleted();

  /**
   * @brief 持有者监听状态转移，然后：
   *           1. 通知给更上层的业务方
   *           2. 进行对应的UI更新
   */
  void SetStateChangeCallback(const StateChangeCallback& callback) { callback_ = callback; }

  constexpr RefreshHeaderState GetState() const { return state_; }

 private:
  RefreshHeaderState state_ = RefreshHeaderState::kIdle;
  StateChangeCallback callback_;

  /**
   * @brief 转移到目标状态，无需转移则返回，非法转移则报错。
   * @param target 目标状态
   * @param conditions 可以发生合法转移的状态
   */
  void Transfer(RefreshHeaderState target, std::initializer_list<RefreshHeaderState> conditions);

  void SetState(RefreshHeaderState state);

  /**
   * 出现了一次非法的状态转移尝试
   */
  void Abort();
};
/**
 * @brief 带刷新状态的HeaderView
 *
 * 列表滑动位置与状态的关系：
 *
 *              ||            ||  可能的状态（包括下边界）：
 *              ||            ||    1. kIdle -- 正常上滑
 *              ||            ||    2. kRefreshing -- 刷新过程中用户主动滑回
 *              ||            ||    3. kCompleted -- 刷新过程中用户主动滑回，然后刷新结束
 *   列表的顶部  ==================
 *              ||            ||  可能的状态：
 *              ||            ||    1. kPulling -- 正常下拉
 *              ||            ||    2. kRefreshing -- 刷新过程中用户主动滑回
 *              ||            ||    3. kCompleted -- 刷新过程中用户主动滑回，然后刷新结束
 * 触发刷新的阈值 ==================可能的状态: kRefreshing / kCompleted -- 常规情况：悬停在此位置刷新直到完成
 *              ||            ||  可能的状态：
 *              ||            ||    1. kReleaseToRefresh -- 正常下拉
 *              ||            ||    2. kRefreshing -- 释放手势，开始刷新
 *              ||            ||    3. kCompleted -- 在列表恢复到阈值前就刷新完成了
 */
class RefreshHeader : public ScrollViewHeader {
 public:
  using RefreshHeaderListener = std::function<void()>;
  /**
   * @brief 下拉过程回调
   * @param percent 下拉百分比 offset/header_height
   */
  virtual void OnPulling(float percent) {}
  /**
   * @brief 返回RefreshHeader当前状态
   */
  constexpr RefreshHeaderState GetState() const { return state_automata_.GetState(); }
  /**
   * @brief 监听刷新控件状态：RefreshHeaderState
   * @param state 要监听的状态
   * @param listener 外部监听器
   */
  uint64_t AddStateListener(RefreshHeaderState state, const RefreshHeaderListener& listener);

  /**
   * @brief 移除 AddStateListener 注册的监听
   */
  void RemoveStateListener(RefreshHeaderState state, uint64_t id);

  /**
   * @brief 完成刷新过程，需要外部实现者主动在刷新完成之后调用
   */
  void FinishRefresh();

 protected:
  void Init() override;
  void OnStateChange(RefreshHeaderState state);
  void OnScroll(const TPoint& old_offset, const TPoint& new_offset) override;
  void OnRelease() override;
  void OnScrollEnd() override;

 private:
  TPoint new_offset_;
  std::map<RefreshHeaderState, std::map<uint64_t, RefreshHeaderListener>> state_listener_;
  RefreshStateAutomata state_automata_;
};

}  // namespace tdfcore
