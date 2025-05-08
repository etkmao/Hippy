/*
 *
 * Tencent is pleased to support the open source community by making
 * Hippy available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "renderer/components/waterfall_view.h"
#include "renderer/components/rich_text_view.h"
#include "renderer/utils/hr_pixel_utils.h"
#include "renderer/utils/hr_value_utils.h"
#include "renderer/native_render_provider.h"
#include "renderer/components/waterfall_item_view.h"
#include "renderer/arkui/native_node_api.h"

namespace hippy {
inline namespace render {
inline namespace native {

WaterfallView::WaterfallView(std::shared_ptr<NativeRenderContext> &ctx) : BaseView(ctx) {
}

WaterfallView::~WaterfallView() {
  ctx_->GetNativeRender().lock()->RemoveEndBatchCallback(ctx_->GetRootId(), end_batch_callback_id_);
  if (flowNode_) {
    flowNode_->ResetLazyAdapter();
  }
  adapter_.reset();
  if (!children_.empty()) {
    children_.clear();
  }
  if (sectionOption_) {
    OH_ArkUI_WaterFlowSectionOption_Dispose(sectionOption_);
  }
  if (headerView_) {
    headerView_->DestroyArkUINode();
  }
}

ArkUINode *WaterfallView::GetLocalRootArkUINode() { 
  return refreshNode_.get();
}

void WaterfallView::CreateArkUINodeImpl() {
  flowNode_ = std::make_shared<WaterFlowNode>();
  flowNode_->SetArkUINodeDelegate(this);
  flowNode_->SetNodeDelegate(this);
  flowNode_->SetSizePercent(HRSize(1.f, 1.f));
  flowNode_->SetScrollBarDisplayMode(ARKUI_SCROLL_BAR_DISPLAY_MODE_OFF);
  flowNode_->SetCachedCount(4);
  flowNode_->SetNestedScroll(ARKUI_SCROLL_NESTED_MODE_SELF_FIRST, ARKUI_SCROLL_NESTED_MODE_SELF_FIRST);
  
  adapter_ = std::make_shared<WaterfallItemAdapter>(children_, hasPullHeader_ ? 1 : 0);
  flowNode_->SetLazyAdapter(adapter_->GetHandle());
  
  flowNode_->SetScrollEdgeEffect(ARKUI_EDGE_EFFECT_NONE);
  
  // TODO(hot):
//  flowNode_->SetColumnGap(6);
//  flowNode_->SetRowGap(6);

  refreshNode_ = std::make_shared<RefreshNode>();
  refreshNode_->SetNodeDelegate(this);
  refreshNode_->SetRefreshPullToRefresh(true);
  refreshNode_->SetRefreshRefreshing(false);
  refreshNode_->SetRefreshPullDownRatio(0);
  refreshNode_->AddChild(flowNode_.get());
}

void WaterfallView::DestroyArkUINodeImpl() {
  flowNode_->SetArkUINodeDelegate(nullptr);
  flowNode_->SetNodeDelegate(nullptr);
  flowNode_->ResetLazyAdapter();
  flowNode_ = nullptr;
  adapter_.reset();
  refreshNode_->SetNodeDelegate(nullptr);
  refreshNode_ = nullptr;
}

bool WaterfallView::SetPropImpl(const std::string &propKey, const HippyValue &propValue) {
  if (propKey == "bounces") {
    auto flag = HRValueUtils::GetBool(propValue, false);
    if (flag) {
      flowNode_->SetScrollEdgeEffect(ARKUI_EDGE_EFFECT_SPRING);
    } else {
      flowNode_->SetScrollEdgeEffect(ARKUI_EDGE_EFFECT_NONE);
    }
    return true;
  } else if (propKey == "contentInset") {
    float top = 0;
    float right = 0;
    float bottom = 0;
    float left = 0;
    HippyValueObjectType data;
    if(propValue.ToObject(data)) {
       top = HRValueUtils::GetFloat(data["top"]);
       bottom = HRValueUtils::GetFloat(data["bottom"]);
       left = HRValueUtils::GetFloat(data["left"]);
       right = HRValueUtils::GetFloat(data["right"]);
    }
    flowNode_->SetPadding(HRPixelUtils::DpToVp(top), HRPixelUtils::DpToVp(right), HRPixelUtils::DpToVp(bottom), HRPixelUtils::DpToVp(left));
    return true;
  } else if (propKey == "scrollEventThrottle") {
    scrollEventThrottle_ = HRValueUtils::GetFloat(propValue, 30);
    return true;
  } else if (propKey == "preloadItemNumber") {
    preloadItemNumber_ = HRValueUtils::GetInt32(propValue);
    return true;
  } else if (propKey == "interItemSpacing") {
    auto v = HRValueUtils::GetFloat(propValue, 0);
    if (v != interItemSpacing_) {
      interItemSpacing_ = v;
      toUpdateSection_ = true;
    }
    return true;
  } else if (propKey == "columnSpacing") {
    auto v = HRValueUtils::GetFloat(propValue, 0);
    if (v != columnSpacing_) {
      columnSpacing_ = v;
      toUpdateSection_ = true;
    }
    return true;
  } else if (propKey == "numberOfColumns") {
    auto v = HRValueUtils::GetInt32(propValue, 2);
    if (v != numberOfColumns_) {
      numberOfColumns_ = v;
      toUpdateSection_ = true;
    }
    return true;
  } else if (propKey == "scroll") {
    onScrollEventEnable_ = HRValueUtils::GetBool(propValue, false);
    return true;
  }
  return BaseView::SetPropImpl(propKey, propValue);
}

void WaterfallView::OnSetPropsEndImpl() {
  if (toUpdateSection_) {
    toUpdateSection_ = false;
    UpdateSectionOption();
  }
  return BaseView::OnSetPropsEndImpl();
}

void WaterfallView::Init() {
  BaseView::Init();
  auto weak_view = weak_from_this();
  end_batch_callback_id_ = ctx_->GetNativeRender().lock()->AddEndBatchCallback(ctx_->GetRootId(), [weak_view]() {
    auto view = weak_view.lock();
    if (view) {
      auto waterfallView = std::static_pointer_cast<WaterfallView>(view);
      waterfallView->HandleOnChildrenUpdated();
      waterfallView->CheckInitListReadyNotify();
    }
  });
}

void WaterfallView::HandleOnChildrenUpdated() {
  auto childrenCount = children_.size();
  if (childrenCount > 0) {
    // Index must be recalculated.
    for (uint32_t i = 0; i < childrenCount; i++) {
      auto itemView = std::static_pointer_cast<WaterfallItemView>(children_[i]);
      auto node = itemView->GetLocalRootArkUINode();
      if (node) {
//        node->SetItemIndex((int32_t)i);
      }
    }
    
    auto firstChild = std::static_pointer_cast<WaterfallItemView>(children_[0]);
    if (firstChild->GetViewType() == PULL_HEADER_VIEW_TYPE) {
      headerView_ = std::static_pointer_cast<WaterfallPullHeaderView>(firstChild);
      hasPullHeader_ = true;
      adapter_->SetFirstItemIndex(1);
      
      headerView_->CreateArkUINode(true, 0);
      auto refreshOffset = headerView_->GetHeight();
      headerView_->SetPosition({0, - refreshOffset});
      
      refreshNode_->SetRefreshPullDownRatio(1);
      refreshNode_->SetRefreshContent(headerView_->GetLocalRootArkUINode()->GetArkUINodeHandle());
      refreshNode_->SetRefreshOffset(refreshOffset);
      
      if (childrenCount > 1) {
        auto theChild = std::static_pointer_cast<WaterfallItemView>(children_[1]);
        if (theChild->GetType() == WaterfallItemView::HEAD_BANNER_TYPE) {
          headBannerView_ = theChild;
        }
      }
    } else if (firstChild->GetType() == WaterfallItemView::HEAD_BANNER_TYPE) {
      headBannerView_ = firstChild;
    }
    
    auto lastChild = std::static_pointer_cast<WaterfallItemView>(children_[childrenCount - 1]);
    if (lastChild->GetViewType() == PULL_FOOTER_VIEW_TYPE) {
      footerView_ = std::static_pointer_cast<WaterfallPullFooterView>(lastChild);
      //footerView_->Show(false);//TODO(hot):
      if (childrenCount > 1) {
        auto theChild = std::static_pointer_cast<WaterfallItemView>(children_[childrenCount - 2]);
        if (theChild->GetType() == WaterfallItemView::FOOT_BANNER_TYPE) {
          footBannerView_ = theChild;
        }
      }
    } else if (lastChild->GetType() == WaterfallItemView::FOOT_BANNER_TYPE) {
      footBannerView_ = lastChild;
    }
  }
  
  UpdateSectionOption();
}

void WaterfallView::OnChildInserted(std::shared_ptr<BaseView> const &childView, int index) {
  BaseView::OnChildInserted(childView, index);
  if (adapter_) {
    adapter_->InsertItem(index);
  }
}

void WaterfallView::OnChildRemoved(std::shared_ptr<BaseView> const &childView, int32_t index) {
  BaseView::OnChildRemoved(childView, index);
  if (adapter_) {
    adapter_->RemoveItem(index);
  }
}

void WaterfallView::OnChildInsertedImpl(std::shared_ptr<BaseView> const &childView, int32_t index) {
  BaseView::OnChildInsertedImpl(childView, index);
  
//  auto itemView = std::static_pointer_cast<ListItemView>(childView);
//  itemView->GetLocalRootArkUINode()->SetNodeDelegate(this);
//  itemView->GetLocalRootArkUINode()->SetItemIndex(index);
}

void WaterfallView::OnChildRemovedImpl(std::shared_ptr<BaseView> const &childView, int32_t index) {
  BaseView::OnChildRemovedImpl(childView, index);
}

void WaterfallView::OnChildReusedImpl(std::shared_ptr<BaseView> const &childView, int index) {
  BaseView::OnChildReusedImpl(childView, index);
  
//  auto itemView = std::static_pointer_cast<ListItemView>(childView);
//  itemView->GetLocalRootArkUINode()->SetItemIndex(index);
}

void WaterfallView::UpdateRenderViewFrameImpl(const HRRect &frame, const HRPadding &padding) {
  BaseView::UpdateRenderViewFrameImpl(frame, padding);
  width_ = frame.width;
  height_ = frame.height;

  // TODO(hot):
//  CheckValidListSize();
}

void WaterfallView::CallImpl(const std::string &method, const std::vector<HippyValue> params,
              std::function<void(const HippyValue &result)> callback) {
  FOOTSTONE_DLOG(INFO) << __FUNCTION__ << " method = " << method;
  if(method == "scrollToIndex") {
    int32_t index = HRValueUtils::GetInt32(params[1]);
    bool animate = HRValueUtils::GetBool(params[2], false);
    flowNode_->ScrollToIndex(index, animate, ARKUI_SCROLL_ALIGNMENT_START);
  } else if (method == "scrollToContentOffset") {
    // 瀑布流组件无相关API
  } else if (method == "scrollToTop") {
    flowNode_->ScrollToIndex(0, true, ARKUI_SCROLL_ALIGNMENT_START);
  } else {
    BaseView::CallImpl(method, params, callback);
  }
}

void WaterfallView::OnWaterFlowScrollIndex(int32_t firstIndex, int32_t lastIndex) {

}

void WaterfallView::OnWaterFlowDidScroll(float_t offset, ArkUI_ScrollState state) {
  FOOTSTONE_DLOG(INFO) << __FUNCTION__;
}

void WaterfallView::OnWaterFlowWillScroll(float_t offset, ArkUI_ScrollState state, int32_t source) {
  FOOTSTONE_DLOG(INFO) << __FUNCTION__;

}

void WaterfallView::OnScroll(float scrollOffsetX, float scrollOffsetY) {
  FOOTSTONE_LOG(INFO) << "xxx hippy, on scroll, x: " << scrollOffsetX << ", y: " << scrollOffsetY;
}

void WaterfallView::OnScrollStart() {
  
}

void WaterfallView::OnScrollStop() {
  
}

void WaterfallView::OnReachStart() {
  
}

void WaterfallView::OnReachEnd() {
  FOOTSTONE_DLOG(INFO) << __FUNCTION__;
  SendOnReachedEvent();
}

void WaterfallView::OnTouch(int32_t actionType, const HRPosition &screenPosition) {
  BaseView::OnTouch(actionType, screenPosition);
  if (actionType == UI_TOUCH_EVENT_ACTION_DOWN || actionType == UI_TOUCH_EVENT_ACTION_MOVE) {
    CheckBeginDrag();
  } else if (actionType == UI_TOUCH_EVENT_ACTION_UP || actionType == UI_TOUCH_EVENT_ACTION_CANCEL) {
    CheckEndDrag();
  }
}

void WaterfallView::CheckBeginDrag() {
  if (!isDragging_) {
    isDragging_ = true;
//    if (scrollBeginDragEventEnable_) {
//      EmitScrollEvent(HREventUtils::EVENT_SCROLLER_BEGIN_DRAG);
//    }
    
  }
}

void WaterfallView::CheckEndDrag() {
  if (isDragging_) {
    isDragging_ = false;
    /*
    if (scrollEndDragEventEnable_) {
      EmitScrollEvent(HREventUtils::EVENT_SCROLLER_END_DRAG);
    }
    if (momentumScrollBeginEventEnable_) {
      EmitScrollEvent(HREventUtils::EVENT_SCROLLER_MOMENTUM_BEGIN);
    }

    if (headerView_ && pullAction_ == ScrollAction::PullHeader) {
      if (headerViewFullVisible_) {
        HREventUtils::SendComponentEvent(headerView_->GetCtx(), headerView_->GetTag(),
                                         HREventUtils::EVENT_PULL_HEADER_RELEASED, nullptr);
      } else {
        listNode_->ScrollToIndex(1, true, ARKUI_SCROLL_ALIGNMENT_START);
      }
      pullAction_ = ScrollAction::None;
    } else if (footerView_ && pullAction_ == ScrollAction::PullFooter) {
      if (footerViewFullVisible_) {
        HREventUtils::SendComponentEvent(footerView_->GetCtx(), footerView_->GetTag(),
                                         HREventUtils::EVENT_PULL_FOOTER_RELEASED, nullptr);
      } else {
        auto lastIndex = static_cast<int32_t>(children_.size()) - 1;
        listNode_->ScrollToIndex(lastIndex - 1, true, ARKUI_SCROLL_ALIGNMENT_END);
      }
      pullAction_ = ScrollAction::None;
    }
    */
  }
}

/*
void WaterfallView::OnScrollIndex(int32_t firstIndex, int32_t lastIndex, int32_t centerIndex){

}

void WaterfallView::OnScroll(float scrollOffsetX, float scrollOffsetY) {
//  auto offset = listNode_->GetScrollOffset();
//  HippyValueObjectType params;
//  if(headerView_ && headerVisible_) {
//    if(isDragging_) {
//      params["contentOffset"] = HRPixelUtils::VpToDp(-offset.y+headerView_->GetHeight());
//      HREventUtils::SendComponentEvent(headerView_->GetCtx(), headerView_->GetTag(),
//                                       HREventUtils::EVENT_PULL_HEADER_PULLING, std::make_shared<HippyValue>(params));
//    } else{
//      HREventUtils::SendComponentEvent(headerView_->GetCtx(), headerView_->GetTag(),
//                                       HREventUtils::EVENT_PULL_HEADER_RELEASED, nullptr);
//    }
//  }
//  if(footerView_ && footerVisible_) {
//    UpdateFooterView();
//  }
}

void WaterfallView::OnWillScroll(float offset, ArkUI_ScrollState state) {
  if (offset > 0) {
    if (footerView_) {
      footerView_->Show(true);
    }
  }
}


*/

void WaterfallView::OnFlowItemVisibleAreaChange(int32_t index, bool isVisible, float currentRatio) {

}

void WaterfallView::OnRefreshing() {
  refreshNode_->SetRefreshRefreshing(true);
  HREventUtils::SendComponentEvent(headerView_->GetCtx(), headerView_->GetTag(),
                                   HREventUtils::EVENT_PULL_HEADER_RELEASED, nullptr);
}

void WaterfallView::OnStateChange(int32_t state) {
  
}

void WaterfallView::OnOffsetChange(float_t offset) {
  auto refreshOffset = headerView_->GetHeight();
  headerView_->SetPosition({0, offset - refreshOffset});
  if (isDragging_) {
    HippyValueObjectType params;
    params["contentOffset"] = HRPixelUtils::VpToDp(offset);
    HREventUtils::SendComponentEvent(headerView_->GetCtx(), headerView_->GetTag(),
                                     HREventUtils::EVENT_PULL_HEADER_PULLING, std::make_shared<HippyValue>(params));
  }
}

//void WaterfallView::OnItemVisibleAreaChange(int32_t index, bool isVisible, float currentRatio) {
//  // FOOTSTONE_DLOG(INFO) << __FUNCTION__ << " index = " << index << " isVisible = " << isVisible;
//  if(headerView_ && index == 0){
//    if(isVisible){
//      headerVisible_ = true;
//    } else{
//      headerVisible_ = false;
//    }
//  }
//  if(footerView_ && index == lastScrollIndex_){
//    if(isVisible){
//      footerVisible_ = true;
//    } else{
//      footerVisible_ = false;
//    }
//  }
//}

void WaterfallView::OnHeadRefreshFinish(int32_t delay) {
  FOOTSTONE_DLOG(INFO) << __FUNCTION__ << " delay = " << delay;
  refreshNode_->SetRefreshRefreshing(false);
}

void WaterfallView::OnHeadRefresh() {
  FOOTSTONE_DLOG(INFO) << __FUNCTION__;
}

void WaterfallView::SendOnReachedEvent(){
  FOOTSTONE_DLOG(INFO) << __FUNCTION__;
  HREventUtils::SendComponentEvent(ctx_, tag_, HREventUtils::EVENT_RECYCLER_END_REACHED, nullptr);
  HREventUtils::SendComponentEvent(ctx_, tag_, HREventUtils::EVENT_RECYCLER_LOAD_MORE, nullptr);
}

void WaterfallView::CheckInitListReadyNotify() {
  if (!isInitListReadyNotified_) {
    HREventUtils::SendComponentEvent(ctx_, tag_, HREventUtils::EVENT_RECYCLER_LIST_READY, nullptr);
    isInitListReadyNotified_ = true;
  }
}

void WaterfallView::UpdateSectionOption() {
  if (!flowNode_ || !children_.size()) {
    return;
  }
  
  if (!sectionOption_) {
    sectionOption_ = OH_ArkUI_WaterFlowSectionOption_Create();
  }
  
  int32_t sectionSize = 0;
  int32_t headBannerIndex = -1;
  int32_t contentIndex = -1;
  int32_t footBannerIndex = -1;
  int32_t footerIndex = -1;
  
  int32_t contentItemCount = (int32_t)children_.size();
  contentItemCount -= (headerView_ ? 1 : 0);
  contentItemCount -= (headBannerView_ ? 1 : 0);
  contentItemCount -= (footBannerView_ ? 1 : 0);
  contentItemCount -= (footerView_ ? 1 : 0);
  
  if (headBannerView_) {
    headBannerIndex = sectionSize;
    ++sectionSize;
  }
  
  if (contentItemCount > 0) {
    contentIndex = sectionSize;
    ++sectionSize;
  }
  
  if (footBannerView_) {
    footBannerIndex = sectionSize;
    ++sectionSize;
  }
  
  if (footerView_) {
    footerIndex = sectionSize;
    ++sectionSize;
  }
  
  OH_ArkUI_WaterFlowSectionOption_SetSize(sectionOption_, sectionSize);
  
  auto colSpace = HRPixelUtils::DpToPx(columnSpacing_);
  auto rowSpace = HRPixelUtils::DpToPx(interItemSpacing_);
  
  if (headBannerView_) {
    OH_ArkUI_WaterFlowSectionOption_SetItemCount(sectionOption_, headBannerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetCrossCount(sectionOption_, headBannerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetColumnGap(sectionOption_, headBannerIndex, colSpace);
    OH_ArkUI_WaterFlowSectionOption_SetRowGap(sectionOption_, headBannerIndex, rowSpace);
    OH_ArkUI_WaterFlowSectionOption_RegisterGetItemMainSizeCallbackByIndexWithUserData(
      sectionOption_, headBannerIndex, this, GetItemMainSizeCallback);
    OH_ArkUI_WaterFlowSectionOption_SetMargin(sectionOption_, headBannerIndex, 0, 0, rowSpace, 0);
  }

  if (contentItemCount > 0) {
    OH_ArkUI_WaterFlowSectionOption_SetItemCount(sectionOption_, contentIndex, contentItemCount);
    OH_ArkUI_WaterFlowSectionOption_SetCrossCount(sectionOption_, contentIndex, numberOfColumns_);
    OH_ArkUI_WaterFlowSectionOption_SetColumnGap(sectionOption_, contentIndex, colSpace);
    OH_ArkUI_WaterFlowSectionOption_SetRowGap(sectionOption_, contentIndex, rowSpace);
    OH_ArkUI_WaterFlowSectionOption_RegisterGetItemMainSizeCallbackByIndexWithUserData(
      sectionOption_, contentIndex, this, GetItemMainSizeCallback);
    OH_ArkUI_WaterFlowSectionOption_SetMargin(sectionOption_, contentIndex, 0, 0, rowSpace, 0);
  }
  
  if (footBannerView_) {
    OH_ArkUI_WaterFlowSectionOption_SetItemCount(sectionOption_, footBannerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetCrossCount(sectionOption_, footBannerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetColumnGap(sectionOption_, footBannerIndex, colSpace);
    OH_ArkUI_WaterFlowSectionOption_SetRowGap(sectionOption_, footBannerIndex, rowSpace);
    OH_ArkUI_WaterFlowSectionOption_RegisterGetItemMainSizeCallbackByIndexWithUserData(
      sectionOption_, footBannerIndex, this, GetItemMainSizeCallback);
  }
  
  if (footerView_) {
    OH_ArkUI_WaterFlowSectionOption_SetItemCount(sectionOption_, footerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetCrossCount(sectionOption_, footerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetColumnGap(sectionOption_, footerIndex, colSpace);
    OH_ArkUI_WaterFlowSectionOption_SetRowGap(sectionOption_, footerIndex, rowSpace);
    OH_ArkUI_WaterFlowSectionOption_RegisterGetItemMainSizeCallbackByIndexWithUserData(
      sectionOption_, footerIndex, this, GetItemMainSizeCallback);
  }
  
  flowNode_->SetSectionOption(sectionOption_);
}

float WaterfallView::GetItemMainSizeCallback(int32_t itemIndex, void* userData) {
  if (!userData) {
    return 0;
  }
  auto view = static_cast<WaterfallView*>(userData);
  int32_t firstIndex = view->hasPullHeader_? 1 : 0;
  if (itemIndex >= 0 && (firstIndex + itemIndex) < ((int32_t)view->children_.size())) {
    auto theChild = std::static_pointer_cast<WaterfallItemView>(view->children_[(size_t)(firstIndex + itemIndex)]);
    return theChild->GetHeight();
  }
  return 0;
}

} // namespace native
} // namespace render
} // namespace hippy
