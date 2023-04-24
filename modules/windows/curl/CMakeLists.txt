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

cmake_minimum_required(VERSION 3.14)

project("curl_wrapper")

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../.." REALPATH)

set(CMAKE_CXX_STANDARD 17)

# region library
add_library(${PROJECT_NAME} STATIC)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})
# endregion

# TODO(charleeshen): modify to hip
# region curl
include(FetchContent)
set(CURL_STATIC_CRT ON)
set(ENABLE_DEBUG ON)
set(BUILD_SHARED_LIBS OFF)
set(CURL_USE_SCHANNEL ON)
set(CMAKE_DEBUG_POSTFIX "")
set(ENABLE_WEBSOCKETS ON)
FetchContent_Declare(curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG curl-7_88_1)
FetchContent_MakeAvailable(curl)
target_include_directories(${PROJECT_NAME} PUBLIC ${curl_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)
# endregion


# region source set
set(SOURCE_SET src/curl_wrapper.cc)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_SET})
# endregion
