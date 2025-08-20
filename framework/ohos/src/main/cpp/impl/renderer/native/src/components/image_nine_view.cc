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

#include "renderer/components/image_nine_view.h"
#include "oh_napi/ark_ts.h"
#include "oh_napi/oh_napi_object.h"
#include "renderer/utils/hr_event_utils.h"
#include "renderer/utils/hr_url_utils.h"
#include "renderer/utils/hr_value_utils.h"
#include "renderer/utils/hr_pixel_utils.h"
#include <native_drawing/drawing_pixel_map.h>
#include <native_drawing/drawing_rect.h>

namespace hippy {
inline namespace render {
inline namespace native {

ImageNineView::ImageNineView(std::shared_ptr<NativeRenderContext> &ctx) : ImageBaseView(ctx) {
}

ImageNineView::~ImageNineView() {}

CustomNode *ImageNineView::GetLocalRootArkUINode() {
  return customNode_.get();
}

void ImageNineView::CreateArkUINodeImpl() {
  customNode_ = std::make_shared<CustomNode>();
  customNode_->SetCustomNodeDelegate(this);
}

void ImageNineView::DestroyArkUINodeImpl() {
  customNode_->SetCustomNodeDelegate(nullptr);
  customNode_ = nullptr;
  ClearProps();
}

bool ImageNineView::RecycleArkUINodeImpl(std::shared_ptr<RecycleView> &recycleView) { // TODO(hot): recycle and reuse
  customNode_->SetCustomNodeDelegate(nullptr);
  customNode_->ResetAllAttributes();
  recycleView->cachedNodes_.resize(1);
  recycleView->cachedNodes_[0] = customNode_;
  customNode_ = nullptr;
  ClearProps();
  return true;
}

bool ImageNineView::ReuseArkUINodeImpl(std::shared_ptr<RecycleView> &recycleView) {
  if (recycleView->cachedNodes_.size() < 1) {
    return false;
  }
  customNode_ = std::static_pointer_cast<CustomNode>(recycleView->cachedNodes_[0]);
  customNode_->SetCustomNodeDelegate(this);
  return true;
}

std::string ImageNineView::GetSrc() {
  return src_;
}

bool ImageNineView::SetPropImpl(const std::string &propKey, const HippyValue &propValue) {
  if (propKey == "src") {
    auto& value = HRValueUtils::GetString(propValue);
    if (value != src_) {
      src_ = value;
      ctx_->GetImageLoader()->LoadImage(value, [WEAK_THIS](bool is_success) {
        DEFINE_AND_CHECK_SELF(ImageNineView)
        self->customNode_->MarkDirty(NODE_NEED_RENDER);
      });
//      FetchImage(value);
    }
    return true;
  } else if (propKey == "defaultSource") {
    auto& value = HRValueUtils::GetString(propValue);
    if (!value.empty()) {
//      FetchAltImage(value);
      return true;
    }
    return false;
  } else if (propKey == "capInsets") {
    HippyValueObjectType m;
    if (propValue.IsObject() && propValue.ToObject(m)) {
      capInsetsLeft_ = HRValueUtils::GetFloat(m["left"]);
      capInsetsTop_ = HRValueUtils::GetFloat(m["top"]);
      capInsetsRight_ = HRValueUtils::GetFloat(m["right"]);
      capInsetsBottom_= HRValueUtils::GetFloat(m["bottom"]);
      return true;
    } else {
      return false;
    }
	}
	return BaseView::SetPropImpl(propKey, propValue);
}

void ImageNineView::UpdateRenderViewFrameImpl(const HRRect &frame, const HRPadding &padding) {
  BaseView::UpdateRenderViewFrameImpl(frame, padding);
}

void ImageNineView::OnForegroundDraw(ArkUI_NodeCustomEvent *event) { // TODO(hot): make dirty
  auto *drawContext = OH_ArkUI_NodeCustomEvent_GetDrawContextInDraw(event);
  if (drawContext == nullptr) {
    return;
  }
  auto *drawingHandle = reinterpret_cast<OH_Drawing_Canvas *>(OH_ArkUI_DrawContext_GetCanvas(drawContext));
  if (drawingHandle == nullptr) {
    return;
  }
  
  ArkUI_IntSize size = OH_ArkUI_DrawContext_GetSize(drawContext);

  auto info = ctx_->GetImageLoader()->GetPixelmapInfo(src_);
  if (info) {
//    // PixelMap中像素的截取区域
//    OH_Drawing_Rect *src = OH_Drawing_RectCreate(0, 0, 600, 400);
//    // 画布中显示的区域
//    OH_Drawing_Rect *dst = OH_Drawing_RectCreate(200, 200, 800, 600);
//    // 采样选项对象
//    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
//      OH_Drawing_FilterMode::FILTER_MODE_LINEAR, OH_Drawing_MipmapMode::MIPMAP_MODE_LINEAR);
//    // 绘制PixelMap
//    OH_Drawing_CanvasDrawPixelMapRect(drawingHandle, info->pixelmap_, src, dst, samplingOptions);
    
//    // PixelMap中分割像素图的中心矩形
//    OH_Drawing_Rect *center = OH_Drawing_RectCreate(capInsetsLeft_, capInsetsTop_, (float)info->width_ - capInsetsRight_, (float)info->height_ - capInsetsBottom_);
//    // 画布中显示的区域
//    OH_Drawing_Rect *dst = OH_Drawing_RectCreate(0, 0, (float)size.width, (float)size.height);
//    OH_Drawing_CanvasDrawPixelMapNine(drawingHandle, info->pixelmap_, center, dst, FILTER_MODE_LINEAR);
    
    // 采样选项对象
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
      OH_Drawing_FilterMode::FILTER_MODE_LINEAR, OH_Drawing_MipmapMode::MIPMAP_MODE_LINEAR);
    
    // left top
    float left = 0;
    float top = 0;
    float right = capInsetsLeft_;
    float bottom = capInsetsTop_;
    float dstLeft = 0;
    float dstTop = 0;
    float dstRight = HRPixelUtils::DpToPx(capInsetsLeft_);
    float dstBottom = HRPixelUtils::DpToPx(capInsetsTop_);
    OH_Drawing_Rect *src = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_Rect *dst = OH_Drawing_RectCreate(dstLeft, dstTop, dstRight, dstBottom);
    OH_Drawing_CanvasDrawPixelMapRect(drawingHandle, info->pixelmap_, src, dst, samplingOptions);
    
    // center top
    left = capInsetsLeft_;
    top = 0;
    right = (float)info->width_ - capInsetsRight_;
    bottom = capInsetsTop_;
    dstLeft = HRPixelUtils::DpToPx(left) - 1;
    dstTop = 0;
    dstRight = (float)size.width - HRPixelUtils::DpToPx(capInsetsRight_) + 1;
    dstBottom = HRPixelUtils::DpToPx(capInsetsTop_);
    dstLeft = dstLeft < 0 ? 0 : dstLeft;
    dstRight = dstRight > (float)size.width ? (float)size.width : dstRight;
    src = OH_Drawing_RectCreate(left, top, right, bottom);
    dst = OH_Drawing_RectCreate(dstLeft, dstTop, dstRight, dstBottom);
    OH_Drawing_CanvasDrawPixelMapRect(drawingHandle, info->pixelmap_, src, dst, samplingOptions);
    
    // right top
    left = (float)info->width_ - capInsetsRight_;
    top = 0;
    right = (float)info->width_;
    bottom = capInsetsTop_;
    dstLeft = (float)size.width - HRPixelUtils::DpToPx(capInsetsRight_);
    dstTop = 0;
    dstRight = (float)size.width;
    dstBottom = HRPixelUtils::DpToPx(capInsetsTop_);
    src = OH_Drawing_RectCreate(left, top, right, bottom);
    dst = OH_Drawing_RectCreate(dstLeft, dstTop, dstRight, dstBottom);
    OH_Drawing_CanvasDrawPixelMapRect(drawingHandle, info->pixelmap_, src, dst, samplingOptions);
    
    // left center
    left = 0;
    top = capInsetsTop_;
    right = capInsetsLeft_;
    bottom = (float)info->height_ - capInsetsBottom_;
    dstLeft = 0;
    dstTop = HRPixelUtils::DpToPx(capInsetsTop_) - 1;
    dstRight = HRPixelUtils::DpToPx(capInsetsLeft_);
    dstBottom = (float)size.height - HRPixelUtils::DpToPx(capInsetsBottom_) + 1;
    dstTop = dstTop < 0 ? 0 : dstTop;
    dstBottom = dstBottom > (float)size.height ? (float)size.height : dstBottom;
    src = OH_Drawing_RectCreate(left, top, right, bottom);
    dst = OH_Drawing_RectCreate(dstLeft, dstTop, dstRight, dstBottom);
    OH_Drawing_CanvasDrawPixelMapRect(drawingHandle, info->pixelmap_, src, dst, samplingOptions);
    
    // center center
    left = capInsetsLeft_;
    top = capInsetsTop_;
    right = (float)info->width_ - capInsetsRight_;
    bottom = (float)info->height_ - capInsetsBottom_;
    dstLeft = HRPixelUtils::DpToPx(capInsetsLeft_) - 1;
    dstTop = HRPixelUtils::DpToPx(capInsetsTop_) - 1;
    dstRight = (float)size.width - HRPixelUtils::DpToPx(capInsetsRight_) + 1;
    dstBottom = (float)size.height - HRPixelUtils::DpToPx(capInsetsBottom_) + 1;
    dstLeft = dstLeft < 0 ? 0 : dstLeft;
    dstRight = dstRight > (float)size.width ? (float)size.width : dstRight;
    dstTop = dstTop < 0 ? 0 : dstTop;
    dstBottom = dstBottom > (float)size.height ? (float)size.height : dstBottom;
    src = OH_Drawing_RectCreate(left, top, right, bottom);
    dst = OH_Drawing_RectCreate(dstLeft, dstTop, dstRight, dstBottom);
    OH_Drawing_CanvasDrawPixelMapRect(drawingHandle, info->pixelmap_, src, dst, samplingOptions);
    
    // right center
    left = (float)info->width_ - capInsetsRight_;
    top = capInsetsTop_;
    right = (float)info->width_;
    bottom = (float)info->height_ - capInsetsBottom_;
    dstLeft = (float)size.width - HRPixelUtils::DpToPx(capInsetsRight_);
    dstTop = HRPixelUtils::DpToPx(capInsetsTop_) - 1;
    dstRight = (float)size.width;
    dstBottom = (float)size.height - HRPixelUtils::DpToPx(capInsetsBottom_) + 1;
    dstTop = dstTop < 0 ? 0 : dstTop;
    dstBottom = dstBottom > (float)size.height ? (float)size.height : dstBottom;
    src = OH_Drawing_RectCreate(left, top, right, bottom);
    dst = OH_Drawing_RectCreate(dstLeft, dstTop, dstRight, dstBottom);
    OH_Drawing_CanvasDrawPixelMapRect(drawingHandle, info->pixelmap_, src, dst, samplingOptions);
    
    // left bottom
    left = 0;
    top = (float)info->height_ - capInsetsBottom_;
    right = capInsetsLeft_;
    bottom = (float)info->height_;
    dstLeft = 0;
    dstTop = (float)size.height - HRPixelUtils::DpToPx(capInsetsBottom_);
    dstRight = HRPixelUtils::DpToPx(capInsetsLeft_);
    dstBottom = (float)size.height;
    src = OH_Drawing_RectCreate(left, top, right, bottom);
    dst = OH_Drawing_RectCreate(dstLeft, dstTop, dstRight, dstBottom);
    OH_Drawing_CanvasDrawPixelMapRect(drawingHandle, info->pixelmap_, src, dst, samplingOptions);
    
    // center bottom
    left = capInsetsLeft_;
    top = (float)info->height_ - capInsetsBottom_;
    right = (float)info->width_ - capInsetsRight_;
    bottom = (float)info->height_;
    dstLeft = HRPixelUtils::DpToPx(capInsetsLeft_) - 1;
    dstTop = (float)size.height - HRPixelUtils::DpToPx(capInsetsBottom_);
    dstRight = (float)size.width - HRPixelUtils::DpToPx(capInsetsRight_) + 1;
    dstBottom = (float)size.height;
    dstLeft = dstLeft < 0 ? 0 : dstLeft;
    dstRight = dstRight > (float)size.width ? (float)size.width : dstRight;
    src = OH_Drawing_RectCreate(left, top, right, bottom);
    dst = OH_Drawing_RectCreate(dstLeft, dstTop, dstRight, dstBottom);
    OH_Drawing_CanvasDrawPixelMapRect(drawingHandle, info->pixelmap_, src, dst, samplingOptions);
    
    // right bottom
    left = (float)info->width_ - capInsetsRight_;
    top = (float)info->height_ - capInsetsBottom_;
    right = (float)info->width_;
    bottom = (float)info->height_;
    dstLeft = (float)size.width - HRPixelUtils::DpToPx(capInsetsRight_);
    dstTop = (float)size.height - HRPixelUtils::DpToPx(capInsetsBottom_);
    dstRight = (float)size.width;
    dstBottom = (float)size.height;
    src = OH_Drawing_RectCreate(left, top, right, bottom);
    dst = OH_Drawing_RectCreate(dstLeft, dstTop, dstRight, dstBottom);
    OH_Drawing_CanvasDrawPixelMapRect(drawingHandle, info->pixelmap_, src, dst, samplingOptions);
    
  }
}

void ImageNineView::ClearProps() {
  src_.clear();
}

void ImageNineView::SetSourcesOrAlt(const std::string &imageUrl, bool isSources) {
  auto bundlePath = ctx_->GetNativeRender().lock()->GetBundlePath();
  auto url = HRUrlUtils::ConvertImageUrl(bundlePath, ctx_->IsRawFile(), ctx_->GetResModuleName(), imageUrl);
//  if (isSources) {
//    GetLocalRootArkUINode()->SetSources(url);
//  } else {
//    GetLocalRootArkUINode()->SetAlt(url);
//  }
}

} // namespace native
} // namespace render
} // namespace hippy
