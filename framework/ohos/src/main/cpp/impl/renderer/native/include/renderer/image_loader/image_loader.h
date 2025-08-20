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

#pragma once

#include "footstone/logging.h"
#include "footstone/hippy_value.h"
#include "renderer/native_render.h"
#include "renderer/text_measure/font_collection_manager.h"
#include "renderer/utils/hr_pixel_utils.h"
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <arkui/styled_string.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_text_declaration.h>
#include <native_drawing/drawing_types.h>
#include <native_drawing/drawing_text_typography.h>
#include <native_drawing/drawing_register_font.h>


namespace hippy {
inline namespace render {
inline namespace native {

using HippyValue = footstone::value::HippyValue;
using HippyValueObjectType = footstone::value::HippyValue::HippyValueObjectType;

using LoadImageCallback = std::function<void(bool is_success)>;

class NativeRenderContext;

class PixelMapInfo {
public:
  uint32_t width_ = 0;
  uint32_t height_ = 0;
  OH_Drawing_PixelMap *pixelmap_ = nullptr;
};

class ImageLoader : public std::enable_shared_from_this<ImageLoader> {
public:
  ImageLoader(uint32_t root_id, std::weak_ptr<NativeRender> &native_render)
    : root_id_(root_id), native_render_(native_render) {}

  void LoadImage(const std::string &uri, const std::shared_ptr<NativeRenderContext> &ctx, LoadImageCallback result_cb);
  void BuildPixmap(const std::string &uri, const std::string &content);
  
  std::shared_ptr<PixelMapInfo> GetPixelmapInfo(const std::string &uri);
  
private:
  uint32_t root_id_ = 0;
  std::weak_ptr<NativeRender> native_render_;
  std::map<std::string, std::shared_ptr<PixelMapInfo>> pixelmapInfoMap_;
};

} // namespace native
} // namespace render
} // namespace hippy
