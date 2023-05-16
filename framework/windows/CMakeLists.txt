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

project("window_framework")

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." REALPATH)

include("${PROJECT_ROOT_DIR}/buildconfig/cmake/InfraPackagesModule.cmake")
include("${PROJECT_ROOT_DIR}/buildconfig/cmake/GlobalPackagesModule.cmake")

set(CMAKE_CXX_STANDARD 17)

# region library
add_library(${PROJECT_NAME} STATIC)
target_include_directories(${PROJECT_NAME} PUBLIC include)
# endregion

# region footstone
GlobalPackages_Add(footstone)
target_link_libraries(${PROJECT_NAME} PRIVATE footstone)
# endregion

# region dom
GlobalPackages_Add(dom)
target_link_libraries(${PROJECT_NAME} PRIVATE dom)
# endregion

# region dom
GlobalPackages_Add(vfs)
target_link_libraries(${PROJECT_NAME} PRIVATE vfs)
# endregion

# region js_driver
add_subdirectory(${PROJECT_ROOT_DIR}/driver/js ${CMAKE_CURRENT_BINARY_DIR}/_deps/driver/js)
target_link_libraries(${PROJECT_NAME} PUBLIC js_driver)
# endregion

# region devtools
if (ENABLE_INSPECTOR)
  GlobalPackages_Add(devtools_backend)
  target_link_libraries(${PROJECT_NAME} PUBLIC devtools_backend)
  GlobalPackages_Add(devtools_integration)
  target_link_libraries(${PROJECT_NAME} PUBLIC devtools_integration)
endif ()
# endregion

# region tdf render manager
add_subdirectory(${PROJECT_ROOT_DIR}/renderer/tdf/common tdf_renderer_common)
target_include_directories(${PROJECT_NAME} PUBLIC tdf_renderer_common)
target_link_libraries(${PROJECT_NAME} PUBLIC tdf_renderer_common)
#endregion

#region sqlite
GlobalPackages_Add(sqlite)
target_link_libraries(${PROJECT_NAME} PUBLIC sqlite)
#endregion

# region asio
InfraPackage_Add(asio
    REMOTE "devtools/backend/third_party/asio/1.22.1/asio.tar.gz"
    LOCAL "third_party/asio")
target_compile_definitions(${PROJECT_NAME}
    # no rtti
    PUBLIC "ASIO_NO_TYPEID"
    # no exception
    PUBLIC "ASIO_NO_EXCEPTIONS"
    PUBLIC "ASIO_DISABLE_ALIGNOF")
target_include_directories(${PROJECT_NAME} PUBLIC ${asio_SOURCE_DIR}/asio/include)
# endregion

# region websocketpp
InfraPackage_Add(websocketpp
    REMOTE "devtools/backend/third_party/websocketpp/develop/git-repo.tgz"
    LOCAL "third_party/websocketpp")
add_compile_definitions(
    # no exception
    "_WEBSOCKETPP_NO_EXCEPTIONS_")
target_include_directories(${PROJECT_NAME} PUBLIC ${websocketpp_SOURCE_DIR})
# endregion

# region asio
InfraPackage_Add(asio
    REMOTE "devtools/backend/third_party/asio/1.22.1/asio.tar.gz"
    LOCAL "third_party/asio")
target_compile_definitions(${PROJECT_NAME}
    # no rtti
    PUBLIC "ASIO_NO_TYPEID"
    # no exception
    PUBLIC "ASIO_NO_EXCEPTIONS"
    PUBLIC "ASIO_DISABLE_ALIGNOF")
target_include_directories(${PROJECT_NAME} PUBLIC ${asio_SOURCE_DIR}/asio/include)
# endregion

# region websocketpp
InfraPackage_Add(websocketpp
    REMOTE "devtools/backend/third_party/websocketpp/develop/git-repo.tgz"
    LOCAL "third_party/websocketpp")
add_compile_definitions(
    # no exception
    "_WEBSOCKETPP_NO_EXCEPTIONS_")
target_include_directories(${PROJECT_NAME} PUBLIC ${websocketpp_SOURCE_DIR})
# endregion

# region source set
set(SOURCE_SET
    src/framework.cc
    src/config.cc
    src/engine.cc
    src/driver/driver.cc
    src/driver/module_dispatcher.cc
    src/adaptor/console/event_viewer_console.cc
    src/adaptor/exception/debug_exception.cc
    src/adaptor/storage/sqlite_storage.cc
    src/adaptor/netinfo/adapter_addresses_net_info.cc
    src/adaptor/network/http_cookie.cc
    src/adaptor/network/http_network.cc
    src/modules/clipboard/clipboard.cc
    src/modules/exception/exception.cc
    src/modules/imageloader/image_loader.cc
    src/modules/storage/storage.cc
    src/modules/network/network.cc
    src/modules/network/websocket.cc
    src/modules/network/websocket_client.cc
    src/modules/network/websocket_listener.cc
    src/modules/netinfo/net_info.cc)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_SET})
# endregion
