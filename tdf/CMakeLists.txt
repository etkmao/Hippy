#
# Tencent is pleased to support the open source community by making
# Hippy available.
#
# Copyright (C) 2022 THL A29 Limited, a Tencent company.
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
cmake_minimum_required(VERSION 3.0)

project(tdf)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME}
  INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

macro(AddLinkLibrary name)
  set(LIB_NAME "${PROJECT_NAME}_${name}")
  add_library(${LIB_NAME} STATIC IMPORTED)
  set_property(TARGET ${LIB_NAME} PROPERTY
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/${name}.lib")
  target_link_libraries(${PROJECT_NAME}
    INTERFACE ${LIB_NAME})
endmacro()

AddLinkLibrary(tdfui)
AddLinkLibrary(tdfcore)
AddLinkLibrary(tdfmetric)
AddLinkLibrary(skia)
AddLinkLibrary(tdfshaper)
AddLinkLibrary(tgfx)
AddLinkLibrary(giflib)
AddLinkLibrary(tdfcodec)
AddLinkLibrary(libEGL.dll)
AddLinkLibrary(libGLESv2.dll)
AddLinkLibrary(TracyClient)

target_link_libraries(tdf INTERFACE )
