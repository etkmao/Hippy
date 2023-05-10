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

#include "modules/imageloader/image_loader.h"

#include <objidlbase.h>
#include <memory>

#include "footstone/logging.h"

namespace hippy {
inline namespace windows {
inline namespace framework {
inline namespace module {

constexpr char kHttpUrl[] = "url";

ImageLoader::~ImageLoader() { Gdiplus::GdiplusShutdown(gdiplus_token_); }

bool ImageLoader::Initial() {
  Gdiplus::GdiplusStartupInput startup_input;
  bool ret = GdiplusStartup(&gdiplus_token_, &startup_input, NULL) != Gdiplus::Ok;
  return ret;
}

void ImageLoader::GetSize(const std::shared_ptr<UriLoader>& uri_loader, const footstone::value::HippyValue& request,
                          std::function<void(const footstone::HippyValue& callback_parameter)> callback) {
  std::string url;
  auto ret = ParserRequestUrl(request, url);
  FOOTSTONE_DCHECK(ret);

  auto loader_callback = [callback](UriLoader::RetCode code, std::unordered_map<std::string, std::string> rsp_meta,
                                    vfs::UriHandler::bytes rsp_content) {
    if (code != UriLoader::RetCode::Success) return;
    if (rsp_content.size() > 0) {
      IStream* stream = nullptr;
      HRESULT ret = CreateStreamOnHGlobal(NULL, TRUE, &stream);
      if (ret != S_OK) {
        FOOTSTONE_DLOG(INFO) << "Image stream create fail!!";
        return;
      }

      ULONG cb_written = 0;
      ret = stream->Write(rsp_content.c_str(), (ULONG)rsp_content.size(), &cb_written);
      if (ret != S_OK || cb_written != rsp_content.size()) {
        FOOTSTONE_DLOG(INFO) << "Image stream write fail!!";
        stream->Release();
        return;
      }

      Gdiplus::Image* image = Gdiplus::Image::FromStream(stream);
      int width = image->GetWidth();
      int height = image->GetHeight();
      FOOTSTONE_DLOG(INFO) << "image width = %d, height = %d" << width << height;

      footstone::HippyValue::HippyValueObjectType callback_parameters;
      callback_parameters.insert({"width", footstone::HippyValue(width)});
      callback_parameters.insert({"height", footstone::HippyValue(height)});
      callback(footstone::HippyValue(callback_parameters));
    } else {
      FOOTSTONE_DLOG(INFO) << "Fail to Get Image!!";
    }
  };
  uri_loader->RequestUntrustedContent(footstone::stringview::string_view(url), {}, loader_callback);
  return;
}

void ImageLoader::Prefetch(const std::shared_ptr<UriLoader>& uri_loader, const footstone::value::HippyValue& request) {
  std::string url;
  auto ret = ParserRequestUrl(request, url);
  FOOTSTONE_DCHECK(ret);

  auto loader_callback = [url](UriLoader::RetCode code, std::unordered_map<std::string, std::string> rsp_meta,
                               vfs::UriHandler::bytes rsp_content) {
    if (code == UriLoader::RetCode::Success) {
      FOOTSTONE_DLOG(INFO) << "Prefetch Image Success !! Url = " << url;
    } else {
      FOOTSTONE_DLOG(INFO) << "Prefetch Image Fail!! Url = " << url;
    }
  };
  uri_loader->RequestUntrustedContent(footstone::stringview::string_view(url), {}, loader_callback);
  return;
}

bool ImageLoader::ParserRequestUrl(const footstone::value::HippyValue& request, std::string& url) {
  footstone::value::HippyValue::HippyValueArrayType parameters;
  bool ret = request.ToArray(parameters);
  if (!ret) return false;
  if (parameters.size() != 1) return false;

  if (parameters[0].IsObject()) {
    footstone::value::HippyValue::HippyValueObjectType objects;
    ret = parameters[0].ToObject(objects);
    if (!ret) return false;
    for (const auto& kv : objects) {
      if (kv.first == kHttpUrl) {
        ret = kv.second.ToString(url);
        return ret;
      }
    }
  } else if (parameters[0].IsString()) {
    return parameters[0].ToString(url);
  }

  return false;
}

}  // namespace module
}  // namespace framework
}  // namespace windows
}  // namespace hippy
