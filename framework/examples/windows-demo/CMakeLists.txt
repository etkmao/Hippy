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

cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_GENERATOR_TOOLSET "clangcl")
set(CMAKE_SYSTEM_VERSION "10.0.22000.0")
set(CMAKE_CXX_STANDARD 17)

project("hippy")

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../.." REALPATH)
include("${PROJECT_ROOT_DIR}/buildconfig/cmake/InfraPackagesModule.cmake")
include("${PROJECT_ROOT_DIR}/buildconfig/cmake/GlobalPackagesModule.cmake")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
# if (WIN32)
  # add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ignore:4099")
  # set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /ignore:4099")
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
# endif()

add_definitions(-DNOMINMAX)
add_definitions(-D_USE_MATH_DEFINES)
add_definitions(-DWIN32_LEAN_AND_MEAN)
# add_definitions(-DNDEBUG)
# add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
set(JS_ENGINE "V8")
set(V8_COMPONENT "10.6.194")
set(ENABLE_INSPECTOR ON)

include_directories(${PROJECT_NAME} PUBLIC include)

# region source set
set(SOURCE_SET
    src/main.cc
    src/demo_window.cc)
# endregion

add_executable(${PROJECT_NAME} WIN32 ${SOURCE_SET})

target_link_libraries(${PROJECT_NAME} PRIVATE winmm imm32 iphlpapi wininet gdiplus)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/assets/index.bundle ${CMAKE_CURRENT_BINARY_DIR}/index.bundle)
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND editbin /SUBSYSTEM:CONSOLE "\$(OUTDIR)\$(ProjectName).exe")
endif()

# region windows framework
add_subdirectory(${PROJECT_ROOT_DIR}/framework/windows window_framework)
target_link_libraries(${PROJECT_NAME} PRIVATE window_framework)
# endregion

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT hippy)
