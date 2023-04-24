//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "tdfui/view/recycler.h"
#include "tdfui/view/scroll_view.h"

namespace tdfcore {
class CustomLayoutView;

enum class Orientation { kVertical, kHorizontal };

enum class ItemAction { kAdd, kRemove };
/**
 * @brief 自定义布局视图数据源
 */
class CustomLayoutViewDataSource {
 public:
  /**
   * @brief 获取item个数
   * @return 个数
   */
  virtual int64_t GetItemCount() = 0;

  /**
   * @brief 获取item
   * CustomLayoutView会优先从回收池里获取item，若回收池里没有该类item，才会执行该方法
   * @param index 索引
   * @return item
   */
  virtual std::shared_ptr<View> GetItem(int64_t index, const std::shared_ptr<CustomLayoutView> &custom_layout_view) = 0;

  /**
   * @brief 更新item
   * CustomLayoutView会在item即将显示时执行该方法
   * 实现者应在此方法确定并设置好该item的大小
   * @param index 索引
   * @param item item
   */
  virtual void UpdateItem(int64_t index, const std::shared_ptr<View> &item,
                          const std::shared_ptr<CustomLayoutView> &custom_layout_view) = 0;

  /**
   * @brief 获取item的类型
   * 用于回收池标识
   * @param index 索引
   * @return item类型
   */
  virtual int64_t GetItemType(int64_t index) = 0;

  /**
   * @brief Item是否吸顶
   * @param index 索引
   * @return
   */
  virtual bool IsItemSticky(int64_t index) { return false; }

  /**
   * @brief 获取开始改变的索引
   * CustomLayout在Reload时会执行该方法，询问数据改变的index，并从该index开始重新排版
   * @return 数据改变的索引
   */
  virtual int64_t GetChangingIndex() { return 0; }
};

/**
 * @brief 自定义布局
 */
class CustomLayout {
 public:
  /**
   * @brief 布局方法
   * @param relayout 是否重新布局
   */
  virtual void Layout(bool relayout) = 0;

  /**
   * @brief 获取指定索引的位置
   * @param index 索引
   */
  virtual TPoint GetPosition(int64_t index) = 0;

  /**
   * @brief 滑动更新方法
   * @param offset 偏移量
   */
  virtual void OnOffsetUpdate(TPoint offset) = 0;

 protected:
  std::shared_ptr<View> GetItem(int64_t index);

  void AddItem(int64_t index, const std::shared_ptr<View> &item, bool reverse = false);

  void RemoveItem(int64_t index, const std::shared_ptr<View> &item);

  void SetCustomLayoutView(const std::weak_ptr<CustomLayoutView> &custom_layout_view) {
    custom_layout_view_ = custom_layout_view;
  }

  std::weak_ptr<CustomLayoutView> custom_layout_view_;

  friend class CustomLayoutView;

  /**
   * @brief 统一的设置Item的Frame信息的入口，方便恢复和更新缓存信息
   */
  void SetItemFrame(int64_t index, const std::shared_ptr<View> &item, TRect frame);

 private:
  // 缓存每个位置的Frame信息，保证属性动画的起始值合理（默认初始位置和大小一般都是0，除了主轴方向的大小）
  std::unordered_map<int64_t, TRect> item_frame_map_;

  std::unordered_map<int64_t, int64_t> item_type_map_;
};

/**
 * @brief 自定义布局视图
 */
class CustomLayoutView : public ScrollView {
  TDF_REFF_CLASS_META(CustomLayoutView)

 public:
  /**
   * @brief Item加入/移出视图时发出回调
   */
  using ItemChangeCallback = std::function<void(int64_t, const std::shared_ptr<View> &, ItemAction)>;

  /**
   * @brief 重加载
   * CustomLayoutView会重新加载数据源并且排版
   */
  void Reload();

  /**
   * @brief 滚动到指定索引
   * @param index 索引
   */
  void ScrollToIndex(int64_t index, bool animated = true);

  /**
   * @brief 设置自定义布局
   * @param custom_layout 自定义布局
   */
  void SetCustomLayout(const std::shared_ptr<CustomLayout> &custom_layout);

  /**
   * @brief 获取自定义布局
   * @return 自定义布局
   */
  constexpr const std::shared_ptr<CustomLayout> &GetCustomLayout() const { return custom_layout_; }

  /**
   * @brief 获取数据源
   * @return 数据源
   */
  constexpr const std::shared_ptr<CustomLayoutViewDataSource> &GetDataSource() const { return data_source_; }

  /**
   * @brief 获取回收站
   * @return 回收站
   */
  constexpr const std::shared_ptr<Recycler> &GetRecycler() const { return recycler_; }

  void Init() override;

  void SetItemChangeCallback(const ItemChangeCallback &callback) { on_item_change_ = callback; }

  const ItemChangeCallback &GetItemChangeCallback() const { return on_item_change_; }

 protected:
  /**
   * @brief 构造方法
   * @param data_source 数据源
   * @param custom_layout 自定义布局（默认为线性布局）
   * @param recycler 回收器
   * @param view_context viewContext
   */
  CustomLayoutView(const std::shared_ptr<CustomLayoutViewDataSource> &data_source,
                   const std::shared_ptr<CustomLayout> &custom_layout,
                   const std::shared_ptr<Recycler> &recycler = TDF_MAKE_SHARED(DefaultRecycler),
                   const std::shared_ptr<ViewContext> &view_context = ViewContext::GetCurrent());
  void Layout() override;
  void OnScrollUpdate(TPoint old_pixels, TPoint new_pixels) override;

  std::shared_ptr<CustomLayoutViewDataSource> data_source_;

  ItemChangeCallback on_item_change_;
  std::shared_ptr<CustomLayout> custom_layout_;
  std::shared_ptr<Recycler> recycler_;
  FRIEND_OF_TDF_ALLOC;
};
}  // namespace tdfcore

TDF_REFL_DEFINE(tdfcore::CustomLayoutView, bases<tdfcore::ScrollView>)
TDF_REFL_END(tdfcore::CustomLayoutView)
