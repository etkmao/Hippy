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

#include "renderer/image_loader/image_loader.h"
#include "footstone/logging.h"
#include "dom/root_node.h"
#include "footstone/string_view.h"
#include "renderer/utils/hr_url_utils.h"
#include "renderer/native_render_context.h"
#include <multimedia/image_framework/image/image_common.h>
#include <multimedia/image_framework/image/image_source_native.h>
#include <multimedia/image_framework/image/pixelmap_native.h>
#include <native_drawing/drawing_pixel_map.h>

using HippyValue = footstone::value::HippyValue;
using DomManager = hippy::dom::DomManager;
using RootNode = hippy::dom::RootNode;
using string_view = footstone::string_view;

namespace hippy {
inline namespace render {
inline namespace native {

void ImageLoader::LoadImage(const std::string &uri, const std::shared_ptr<NativeRenderContext> &ctx, LoadImageCallback result_cb) {
  auto &root_map = RootNode::PersistentMap();
  std::shared_ptr<RootNode> root_node;
  bool ret = root_map.Find(root_id_, root_node);
  if (!ret) {
    FOOTSTONE_DLOG(WARNING) << "LoadImage root_node is nullptr";
    return;
  }
  
  std::shared_ptr<DomManager> dom_manager = root_node->GetDomManager().lock();
  if (dom_manager == nullptr) {
    FOOTSTONE_DLOG(WARNING) << "LoadImage dom_manager is nullptr";
    return;
  }

  auto render = native_render_.lock();
  if (!render) {
    return;
  }
  auto loader = render->GetUriLoader().lock();
  FOOTSTONE_CHECK(loader);
  if (!loader) {
    return;
  }
  
  // hpfile://./assets/bg.png
  auto bundlePath = ctx->GetNativeRender().lock()->GetBundlePath();
  auto imageUrl = HRUrlUtils::ConvertImageUrl(bundlePath, ctx->IsRawFile(), ctx->GetResModuleName(), uri);
  
  

  auto cb = [WEAK_THIS, uri, result_cb](UriLoader::RetCode ret_code,
                        const std::unordered_map<std::string, std::string>&,
                        UriLoader::bytes content) {
    FOOTSTONE_LOG(INFO) << "xxx hippy, load image, data len: " << content.length() << ", uri: " << uri;
    // in main thread
  
    DEFINE_AND_CHECK_SELF(ImageLoader)
    self->BuildPixmap(uri, content);
    
    result_cb(true);
  };

  std::vector<std::function<void()>> ops;
  ops.emplace_back([dom_manager, root_node, loader, imageUrl, cb] {
    string_view url_str(imageUrl);
    loader->RequestUntrustedContent(url_str, {}, cb);
  });
  dom_manager->PostTask(Scene(std::move(ops)));
}

void ImageLoader::BuildPixmap(const std::string &uri, const std::string &content) {
  // 获取解码能力范围。
//    Image_MimeType* mimeType = nullptr;
//    size_t length = 0;
//    Image_ErrorCode errCode = OH_ImageSourceNative_GetSupportedFormats(&mimeType, &length);
//    if (errCode != IMAGE_SUCCESS) {
//        OH_LOG_ERROR(LOG_APP, "ImageSourceNativeCTest sourceTest OH_ImageSourceNative_GetSupportedFormats failed, errCode: %{public}d.", errCode);
//        return getJsResult(env, errCode);
//    }
//    for (size_t count = 0; count < length; count++) {
//        OH_LOG_INFO(LOG_APP, "Decode supportedFormats:%{public}s", mimeType[count].data);
//    }
  
  // 创建ImageSource实例
  OH_ImageSourceNative *source = nullptr;
//  Image_ErrorCode errCode = OH_ImageSourceNative_CreateFromUri(name, nameSize, &source);
  Image_ErrorCode errCode = OH_ImageSourceNative_CreateFromData((uint8_t*)content.c_str(), content.size(), &source);
  if (errCode != IMAGE_SUCCESS) {
    FOOTSTONE_LOG(ERROR) << "ImageSourceNative create failed, errCode: " << errCode;
    return;
  }

  // 创建定义图片信息的结构体对象，并获取图片信息
  OH_ImageSource_Info *imageInfo = nullptr;
  OH_ImageSourceInfo_Create(&imageInfo);
  errCode = OH_ImageSourceNative_GetImageInfo(source, 0, imageInfo);
  if (errCode != IMAGE_SUCCESS) {
    FOOTSTONE_LOG(ERROR) << "ImageSourceNative get image info failed, errCode: " << errCode;
    return;
  }

  // 获取指定属性键的值
  uint32_t width = 0, height = 0;
  OH_ImageSourceInfo_GetWidth(imageInfo, &width);
  OH_ImageSourceInfo_GetHeight(imageInfo, &height);
  OH_ImageSourceInfo_Release(imageInfo);
  FOOTSTONE_LOG(INFO) << "ImageSourceNative get image info success, w: " << width << ", h: " << height;
  
//  Image_String getKey;
//  const std::string PIXEL_X_DIMENSION = "PixelXDimension";
//  getKey.data = (char *)PIXEL_X_DIMENSION.c_str();
//  getKey.size = PIXEL_X_DIMENSION.length();
//  Image_String getValue;
//  errCode = OH_ImageSourceNative_GetImageProperty(source, &getKey, &getValue);
//  if (errCode != IMAGE_SUCCESS) {
//    FOOTSTONE_LOG(ERROR) << "ImageSourceNative get image prop failed, errCode: " << errCode;
//    return;
//  }

  // 修改指定属性键的值
//  Image_String setKey;
//  const std::string ORIENTATION = "Orientation";
//  setKey.data = (char *)ORIENTATION.c_str();
//  setKey.size = ORIENTATION.length();
//  Image_String setValue;
//  setValue.data = (char *)"4";
//  setValue.size = 1;
//  errCode = OH_ImageSourceNative_ModifyImageProperty(source, &setKey, &setValue);
//  if (errCode != IMAGE_SUCCESS) {
//    FOOTSTONE_LOG(ERROR) << "ImageSourceNative modify image prop failed, errCode: " << errCode;
//    return;
//  }

  // 通过图片解码参数创建PixelMap对象
  OH_DecodingOptions *ops = nullptr;
  OH_DecodingOptions_Create(&ops);
  // 设置为AUTO会根据图片资源格式解码，如果图片资源为HDR资源则会解码为HDR的pixelmap
  OH_DecodingOptions_SetDesiredDynamicRange(ops, IMAGE_DYNAMIC_RANGE_AUTO);
  OH_PixelmapNative *resPixMap = nullptr;

  // ops参数支持传入nullptr, 当不需要设置解码参数时，不用创建
  errCode = OH_ImageSourceNative_CreatePixelmap(source, ops, &resPixMap);
  OH_DecodingOptions_Release(ops);
  if (errCode != IMAGE_SUCCESS) {
    FOOTSTONE_LOG(ERROR) << "ImageSourceNative create pixmap failed, errCode: " << errCode;
    return;
  }
  
  if (resPixMap) {
    OH_Drawing_PixelMap *pixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(resPixMap);
    auto info = std::make_shared<PixelMapInfo>();
    info->width_ = width;
    info->height_ = height;
    info->pixelmap_ = pixelMap;
    pixelmapInfoMap_[uri] = info;
    return;
  }

  // 判断pixelmap是否为hdr内容
  OH_Pixelmap_ImageInfo *pixelmapImageInfo = nullptr;
  OH_PixelmapImageInfo_Create(&pixelmapImageInfo);
  OH_PixelmapNative_GetImageInfo(resPixMap, pixelmapImageInfo);
  bool pixelmapIsHdr = false;
  OH_PixelmapImageInfo_GetDynamicRange(pixelmapImageInfo, &pixelmapIsHdr);
  OH_PixelmapImageInfo_Release(pixelmapImageInfo);

  // 获取图像帧数
  uint32_t frameCnt = 0;
  errCode = OH_ImageSourceNative_GetFrameCount(source, &frameCnt);
  if (errCode != IMAGE_SUCCESS) {
    FOOTSTONE_LOG(ERROR) << "ImageSourceNative get frame count failed, errCode: " << errCode;
    return;
  }

  // 通过图片解码参数创建Pixelmap列表。
  OH_DecodingOptions *opts = nullptr;
  OH_DecodingOptions_Create(&opts);
  OH_PixelmapNative **resVecPixMap = new OH_PixelmapNative*[frameCnt];
  size_t outSize = frameCnt;
  errCode = OH_ImageSourceNative_CreatePixelmapList(source, opts, resVecPixMap, outSize);
  OH_DecodingOptions_Release(opts);
  delete[] resVecPixMap;
  if (errCode != IMAGE_SUCCESS) {
    FOOTSTONE_LOG(ERROR) << "ImageSourceNative create pixmap list failed, errCode: " << errCode;
    return;
  }

  // 获取图像延迟时间列表
  int32_t *delayTimeList = new int32_t[frameCnt];
  size_t size = frameCnt;
  errCode = OH_ImageSourceNative_GetDelayTimeList(source, delayTimeList, size);
  delete[] delayTimeList;
  if (errCode != IMAGE_SUCCESS) {
    FOOTSTONE_LOG(ERROR) << "ImageSourceNative get delay time list failed, errCode: " << errCode;
    return;
  }

  // 释放ImageSource实例
  OH_ImageSourceNative_Release(source);
}

std::shared_ptr<PixelMapInfo> ImageLoader::GetPixelmapInfo(const std::string &uri) {
  auto it = pixelmapInfoMap_.find(uri);
  return it != pixelmapInfoMap_.end() ? it->second : nullptr;
}

} // namespace native
} // namespace render
} // namespace hippy
