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
//  if (stackNode_) {
//    stackNode_->UnregisterAppearEvent();
//    stackNode_->UnregisterDisappearEvent();
//  }
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
}

ArkUINode *WaterfallView::GetLocalRootArkUINode() { return flowNode_.get(); } //stackNode_.get(); }

void WaterfallView::CreateArkUINodeImpl() {
//  stackNode_ = std::make_shared<StackNode>();
//  listNode_ = std::make_shared<ListNode>();
//  colInnerNode_ = std::make_shared<ColumnNode>();
//  flowListNode_ = std::make_shared<ListItemNode>();
  
//  bannerListNode_ = std::make_shared<ListItemNode>();

//  stackNode_->RegisterAppearEvent();
//  stackNode_->RegisterDisappearEvent();
//  stackNode_->SetArkUINodeDelegate(this);
  
//  listNode_->SetNodeDelegate(this);
  
  flowNode_ = std::make_shared<WaterFlowNode>();
  flowNode_->SetArkUINodeDelegate(this);
  flowNode_->SetNodeDelegate(this);
  flowNode_->SetSizePercent(HRSize(1.f, 1.f));
//  flowNode_->SetScrollBarDisplayMode(ARKUI_SCROLL_BAR_DISPLAY_MODE_OFF);
  flowNode_->SetCachedCount(4);
  flowNode_->SetNestedScroll(ARKUI_SCROLL_NESTED_MODE_SELF_FIRST, ARKUI_SCROLL_NESTED_MODE_SELF_FIRST);
  
  adapter_ = std::make_shared<WaterfallItemAdapter>(children_);
  flowNode_->SetLazyAdapter(adapter_->GetHandle());
  
//  flowNode_->SetEnableScrollInteraction(true);
  flowNode_->SetScrollEdgeEffect(this->edgeEffect_); // TODO(hot):
//  flowNode_->SetColumnGap(this->columnSpacing_);
//  flowNode_->SetRowGap(this->interItemSpacing_);
//  flowNode_->SetColumnsTemplate(this->columnsTemplate_);

}

void WaterfallView::DestroyArkUINodeImpl() {
//  stackNode_->SetArkUINodeDelegate(nullptr);
  
//  listNode_->SetNodeDelegate(nullptr);

//  stackNode_ = nullptr;
//  listNode_ = nullptr;
//  colInnerNode_ = nullptr;
//  flowListNode_ = nullptr;
  
//  bannerListNode_ = nullptr;
  
  flowNode_->SetArkUINodeDelegate(nullptr);
  flowNode_->SetNodeDelegate(nullptr);
  flowNode_->ResetLazyAdapter();
  flowNode_ = nullptr;
  adapter_.reset();
}

bool WaterfallView::SetPropImpl(const std::string &propKey, const HippyValue &propValue) {
  if (propKey == "bounces") {
    int32_t type = HRValueUtils::GetInt32(propValue, 0);
    if(type == 0)
      this->edgeEffect_ = ArkUI_EdgeEffect::ARKUI_EDGE_EFFECT_SPRING;
    else if (type == 1)
      this->edgeEffect_ = ArkUI_EdgeEffect::ARKUI_EDGE_EFFECT_FADE;
    else if(type == 2)
      this->edgeEffect_ = ArkUI_EdgeEffect::ARKUI_EDGE_EFFECT_NONE;
    return true;
  } else if (propKey == "contentInset") {
    HippyValueObjectType data;
    if(propValue.ToObject(data)) {
       this->padding_.paddingTop = HRValueUtils::GetFloat(data["top"]);
       this->padding_.paddingBottom = HRValueUtils::GetFloat(data["bottom"]);
       this->padding_.paddingLeft = HRValueUtils::GetFloat(data["left"]);
       this->padding_.paddingRight = HRValueUtils::GetFloat(data["right"]);
    } else {
       this->padding_= HRPadding(0,0,0,0);
    }
    return true;
  } else if (propKey == "scrollEventThrottle") {
    this->scrollEventThrottle_ = HRValueUtils::GetFloat(propValue, 30);
    return true;
  } else if (propKey == "preloadItemNumber") {
    this->preloadItemNumber_ = HRValueUtils::GetInt32(propValue);
    return true;
  } else if (propKey == "interItemSpacing") {
    this->interItemSpacing_ = HRValueUtils::GetFloat(propValue,0);
    return true;
  } else if (propKey == "columnSpacing") {
    this->columnSpacing_ = HRValueUtils::GetFloat(propValue, 0);
    return true;
  } else if (propKey == "numberOfColumns") {
    int columns = (int)HRValueUtils::GetDouble(propValue, 2);
    this->columnsTemplate_ = "1fr";
    for(int i = 1 ; i < columns ; i++){
      this->columnsTemplate_ += " 1fr";
    }
    return true;
  } else if (propKey == "scroll") {
    scrollEnable_ = HRValueUtils::GetBool(propValue, false);
    return true;
  } else if (propKey == "endreached") {
    return true;
  }
  return BaseView::SetPropImpl(propKey, propValue);
}

void WaterfallView::OnSetPropsEndImpl(){
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
        node->SetItemIndex((int32_t)i);
      }
    }
    
    auto firstChild = std::static_pointer_cast<WaterfallItemView>(children_[0]);
    if (firstChild->GetViewType() == PULL_HEADER_VIEW_TYPE) {
      headerView_ = std::static_pointer_cast<WaterfallPullHeaderView>(firstChild);
      hasPullHeader_ = true;
      //pullHeaderWH_ = isVertical_ ? headerView_->GetHeight() : headerView_->GetWidth();
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
      //footerView_->Show(false);
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
  
  
//  colInnerNode_->SetPadding(
//    HRPixelUtils::DpToVp(this->padding_.paddingTop),
//    HRPixelUtils::DpToVp(this->padding_.paddingRight),
//    HRPixelUtils::DpToVp(this->padding_.paddingBottom),
//    HRPixelUtils::DpToVp(this->padding_.paddingLeft));
  
  // TODO(hot):
//  flowNode_->SetWidthPercent(1.0);
//  flowNode_->SetScrollEdgeEffect(this->edgeEffect_);
//  flowNode_->SetColumnGap(this->columnSpacing_);
//  flowNode_->SetRowGap(this->interItemSpacing_);
//  flowNode_->SetColumnsTemplate(this->columnsTemplate_);
//  flowNode_->SetCachedCount(4);
//  flowNode_->SetScrollEnableInteraction(true);
//  flowNode_->SetNestedScroll(ARKUI_SCROLL_NESTED_MODE_PARENT_FIRST, ARKUI_SCROLL_NESTED_MODE_SELF_FIRST);
  
//  stackNode_->AddChild(colInnerNode_.get());
//  colInnerNode_->AddChild(listNode_.get());
  
  
//  if(this->headBannerView_) {
//    this->headBannerView_->GetLocalRootArkUINode()->SetPosition(HRPosition(0,0));
//  }

//  if (headerView_) {
//    listNode_->InsertChild(headerView_->GetLocalRootArkUINode(),0);
//    listNode_->InsertChild(bannerListNode_.get(), 1);
//    headerView_->GetLocalRootArkUINode()->SetNodeDelegate(this);
//    headerView_->GetLocalRootArkUINode()->SetItemIndex(0);
//    listNode_->ScrollToIndex(1, true, ARKUI_SCROLL_ALIGNMENT_START);
//  } else{
//    listNode_->InsertChild(bannerListNode_.get(), 0);
//    listNode_->ScrollToIndex(0, true, ARKUI_SCROLL_ALIGNMENT_START);
//  }

//  listNode_->AddChild(flowListNode_.get());
//  flowListNode_->AddChild(flowNode_.get());
//  if(footerView_) {
//    listNode_->AddChild(footerView_->GetLocalRootArkUINode());
//    footerView_->GetLocalRootArkUINode()->SetWidthPercent(1.0);
//    footerView_->Show(false);
//    footerView_->GetLocalRootArkUINode()->SetNodeDelegate(this);
//    lastScrollIndex_ = (int32_t)listNode_->GetTotalChildCount()-1;
//    footerView_->GetLocalRootArkUINode()->SetItemIndex(lastScrollIndex_);
//    UpdateFooterView();
//  }
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

  if (index == 0 && childView->GetViewType() == PULL_HEADER_VIEW_TYPE) {
    flowNode_->ScrollToIndex(1, true, ARKUI_SCROLL_ALIGNMENT_START);
  }
  
//  if (index == 0 && childView->GetViewType() == PULL_HEADER_VIEW_TYPE) {
//    listNode_->SetListInitialIndex(1);
//  }
//  
//  auto itemView = std::static_pointer_cast<ListItemView>(childView);
//  itemView->GetLocalRootArkUINode()->SetNodeDelegate(this);
//  itemView->GetLocalRootArkUINode()->SetItemIndex(index);
  
  /*
  if(childView->GetViewType() == "PullHeaderView") {
    this->headerView_ = std::dynamic_pointer_cast<PullHeaderView>(childView);
  } else if(childView->GetViewType() == "PullFooterView") {
    this->footerView_ = std::dynamic_pointer_cast<PullFooterView>(childView);
  } else if (childView->GetViewType() == "View") {
    if (!this->headBannerView_) {
      this->headBannerView_ = std::dynamic_pointer_cast<DivView>(childView);
//      if(this->headBannerView_) {
//        bannerListNode_->AddChild(this->headBannerView_->GetLocalRootArkUINode());
//      }
      
      ArkUI_WaterFlowSectionOption *option = OH_ArkUI_WaterFlowSectionOption_Create();
      OH_ArkUI_WaterFlowSectionOption_SetSize(option, 2);
      OH_ArkUI_WaterFlowSectionOption_SetItemCount(option, 0, 1);
      OH_ArkUI_WaterFlowSectionOption_SetCrossCount(option, 0, 1);
      
      auto callback = [](int32_t itemIndex, void* userData) -> float {
        return 100.f;
      };
      OH_ArkUI_WaterFlowSectionOption_RegisterGetItemMainSizeCallbackByIndexWithUserData(option, 0, nullptr, callback);
      
      ArkUI_NumberValue value[] = {{.i32 = 0},}; 
      ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, option};
      NativeNodeApi::GetInstance()->setAttribute(flowNode_->GetArkUINodeHandle(), NODE_WATER_FLOW_SECTION_OPTION, &item);
      //OH_ArkUI_WaterFlowSectionOption_Dispose(option);
      
    }
  } else if (childView->GetViewType() == "WaterfallItem") {
    auto flowItem = std::dynamic_pointer_cast<WaterfallItemView>(childView);
    flowItem->GetLocalRootArkUINode()->SetNodeDelegate(this);
    flowItem->GetLocalRootArkUINode()->SetItemIndex(index);
    flowNode_->AddChild(childView->GetLocalRootArkUINode());
  } else {
    FOOTSTONE_DLOG(INFO) << __FUNCTION__ << " new child index = " << index;
  }
  //*/
}

void WaterfallView::OnChildRemovedImpl(std::shared_ptr<BaseView> const &childView, int32_t index) {
  BaseView::OnChildRemovedImpl(childView, index);
//  flowNode_->RemoveChild(childView->GetLocalRootArkUINode());
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
    flowNode_->ScrollToIndex(index, animate);
  } else if (method == "scrollToContentOffset") {

  } else if (method == "scrollToTop") {
//    listNode_->ScrollToIndex(1, true, ARKUI_SCROLL_ALIGNMENT_START);
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
  UpdateFooterView();
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
    
//    UpdateSectionOption();
//    flowNode_->ScrollTo(0, -100, false);
  }
  
  UpdateSectionOption();
  
  auto off = flowNode_->GetScrollOffset();
  FOOTSTONE_LOG(INFO) << "xxx hippy, on drag, off x: " << off.x << ", y: " << off.y;
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

void WaterfallView::OnAppear() {
//  HandleOnChildrenUpdated();
}

void WaterfallView::OnDisappear() {
//  ctx_->GetNativeRender().lock()->RemoveEndBatchCallback(ctx_->GetRootId(), end_batch_callback_id_);
}

void WaterfallView::OnFlowItemVisibleAreaChange(int32_t index, bool isVisible, float currentRatio) {

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
  if(delay > 0) {
    //TODO setTimeout(delay)
//    listNode_->ScrollToIndex(1, true, ARKUI_SCROLL_ALIGNMENT_START);
  }
}

void WaterfallView::OnHeadRefresh() {
  FOOTSTONE_DLOG(INFO) << __FUNCTION__;
}

void WaterfallView::SendOnReachedEvent(){
  FOOTSTONE_DLOG(INFO) << __FUNCTION__;
  HREventUtils::SendComponentEvent(ctx_, tag_, HREventUtils::EVENT_RECYCLER_END_REACHED, nullptr);
  HREventUtils::SendComponentEvent(ctx_, tag_, HREventUtils::EVENT_RECYCLER_LOAD_MORE, nullptr);
}

//void WaterfallView::OnScrollStart() {
//  FOOTSTONE_DLOG(INFO) <<__FUNCTION__;
//}
//
//void WaterfallView::OnScrollStop() {
//  FOOTSTONE_DLOG(INFO) <<__FUNCTION__;
//}
//
//void WaterfallView::OnReachStart() {
//}
//


void WaterfallView::UpdateFooterView(){
  if(footerView_) {
//    auto childrens = footerView_->GetChildren();
//    for(uint64_t i = 0; i < childrens.size();i++) {
//      if(childrens[i]->GetViewType() == "Text"){
//         auto textView = std::dynamic_pointer_cast<RichTextView>(childrens[i]);
//         if(textView) {
//           textView->GetLocalRootArkUINode()->SetPosition(HRPosition(0,0));
//         }
//      }
//    }
  }
}

void WaterfallView::CheckInitListReadyNotify() {
  if (!isInitListReadyNotified_) {
    HREventUtils::SendComponentEvent(ctx_, tag_, HREventUtils::EVENT_RECYCLER_LIST_READY, nullptr);
    isInitListReadyNotified_ = true;
  }
}

void WaterfallView::UpdateSectionOption() {
  if (!sectionOption_) {
    sectionOption_ = OH_ArkUI_WaterFlowSectionOption_Create();
  }
  
  int32_t sectionSize = 0;
  int32_t headerIndex = -1;
  int32_t headBannerIndex = -1;
  int32_t contentIndex = -1;
  int32_t footBannerIndex = -1;
  int32_t footerIndex = -1;
  
  int32_t contentItemCount = (int32_t)children_.size();
  contentItemCount -= (headerView_ ? 1 : 0);
  contentItemCount -= (headBannerView_ ? 1 : 0);
  contentItemCount -= (footBannerView_ ? 1 : 0);
  contentItemCount -= (footerView_ ? 1 : 0);
  
  if (headerView_) {
    headerIndex = sectionSize;
    ++sectionSize;
  }
  
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
  
  if (headerView_) {
    OH_ArkUI_WaterFlowSectionOption_SetItemCount(sectionOption_, headerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetCrossCount(sectionOption_, headerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetColumnGap(sectionOption_, headerIndex, 2.f); // TODO(hot):
    OH_ArkUI_WaterFlowSectionOption_SetRowGap(sectionOption_, headerIndex, 2.f);
    OH_ArkUI_WaterFlowSectionOption_RegisterGetItemMainSizeCallbackByIndexWithUserData(
      sectionOption_, headerIndex, this, GetItemMainSizeCallback);
  }
  
  if (headBannerView_) {
    OH_ArkUI_WaterFlowSectionOption_SetItemCount(sectionOption_, headBannerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetCrossCount(sectionOption_, headBannerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetColumnGap(sectionOption_, headBannerIndex, 2.f);
    OH_ArkUI_WaterFlowSectionOption_SetRowGap(sectionOption_, headBannerIndex, 2.f);
    OH_ArkUI_WaterFlowSectionOption_RegisterGetItemMainSizeCallbackByIndexWithUserData(
      sectionOption_, headBannerIndex, this, GetItemMainSizeCallback);
  }

  if (contentItemCount > 0) {
    OH_ArkUI_WaterFlowSectionOption_SetItemCount(sectionOption_, contentIndex, contentItemCount);
    OH_ArkUI_WaterFlowSectionOption_SetCrossCount(sectionOption_, contentIndex, 2); // TODO(hot):
    OH_ArkUI_WaterFlowSectionOption_SetColumnGap(sectionOption_, contentIndex, 2.f);
    OH_ArkUI_WaterFlowSectionOption_SetRowGap(sectionOption_, contentIndex, 2.f);
    OH_ArkUI_WaterFlowSectionOption_RegisterGetItemMainSizeCallbackByIndexWithUserData(
      sectionOption_, contentIndex, this, GetItemMainSizeCallback);
  }
  
  if (footBannerView_) {
    OH_ArkUI_WaterFlowSectionOption_SetItemCount(sectionOption_, footBannerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetCrossCount(sectionOption_, footBannerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetColumnGap(sectionOption_, footBannerIndex, 2.f);
    OH_ArkUI_WaterFlowSectionOption_SetRowGap(sectionOption_, footBannerIndex, 2.f);
    OH_ArkUI_WaterFlowSectionOption_RegisterGetItemMainSizeCallbackByIndexWithUserData(
      sectionOption_, footBannerIndex, this, GetItemMainSizeCallback);
  }
  
  if (footerView_) {
    OH_ArkUI_WaterFlowSectionOption_SetItemCount(sectionOption_, footerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetCrossCount(sectionOption_, footerIndex, 1);
    OH_ArkUI_WaterFlowSectionOption_SetColumnGap(sectionOption_, footerIndex, 2.f);
    OH_ArkUI_WaterFlowSectionOption_SetRowGap(sectionOption_, footerIndex, 2.f);
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
  
//  if (view->isFirst__) {
//    view->isFirst__ = false;
//    view->offValue_ = 0;
//    if (itemIndex == 0) return 0;
//  }
  
//  view->offValue_ = view->offValue_ + 2;
//  if (itemIndex == 0) return (float)view->offValue_;
  
  if (itemIndex >= 0 && itemIndex < (int32_t)view->children_.size()) {
    auto theChild = std::static_pointer_cast<WaterfallItemView>(view->children_[(size_t)itemIndex]);
    return theChild->GetHeight();
  }
  return 0;
}

} // namespace native
} // namespace render
} // namespace hippy
