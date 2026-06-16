cmake_minimum_required(VERSION 3.13)

# Save C2A_CORE_VERSION before project() command (project() can reset cache variables)
if(DEFINED C2A_CORE_VERSION)
  set(_C2A_CORE_VERSION_SAVED "${C2A_CORE_VERSION}")
endif()

project(C2A_CORE)

# Restore C2A_CORE_VERSION after project()
if(DEFINED _C2A_CORE_VERSION_SAVED)
  set(C2A_CORE_VERSION "${_C2A_CORE_VERSION_SAVED}" CACHE STRING "C2A Core version" FORCE)
  unset(_C2A_CORE_VERSION_SAVED)
endif()

# Build option
## C2A compile config
option(C2A_BUILD_AS_UTF8            "Build C2A as UTF-8" ON)
option(C2A_BUILD_AS_C99             "Build C2A as C99" OFF)
option(C2A_BUILD_AS_CXX             "Build C2A as C++" OFF)

option(C2A_BUILD_FOR_32BIT          "Build C2A for 32bit target(this will add -m32 option)" ON)

## C2A
option(C2A_USE_STDINT_WRAPPER       "Use stdint.h wrapper for C89" OFF)

option(C2A_BUILD_FOR_SILS           "Build C2A for SILS target" ON)

## C2A core library select
option(C2A_USE_ALL_CORE_APPS        "Use C2A-core all Applications" ON)
option(C2A_USE_ALL_CORE_TEST_APPS   "Use C2A-core all Test Applications" ON)
option(C2A_USE_ALL_CORE_LIB         "Use C2A-core all library" ON)
option(C2A_USE_SPACE_PACKET_AS_COMMON_PACKET  "Use C2A-core Space Packet as Common Packet implementation" ON)

## C2A build target board option （標準実装などがこのオプションによってスイッチされる．デフォルトでは sub OBC を想定）
option(C2A_MOBC_FEATURES            "Enable some features for MOBC" OFF)

## C2A CCSDS select
# See also; /docs/core/ccsds.md
option(C2A_USE_CORE_CCSDS_AOS_SPACE_DATA_LINK_PROTOCOL  "Use C2A-core CCSDS AOS SPACE DATA LINK PROTOCOL implementation" OFF)
option(C2A_USE_CORE_CCSDS_TC_SPACE_DATA_LINK_PROTOCOL   "Use C2A-core CCSDS TC SPACE DATA LINK PROTOCOL implementation" OFF)

option(C2A_USE_SIMPLE_LIBC          "Use C2A-core hosted simple libc (c2a-core/library/libc) implementation" OFF)

set(C2A_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# C2A user ディレクトリの検出
# 優先順位: 1. CMake define 2. 環境変数 3. レガシーパス
if(NOT DEFINED C2A_USER_DIR)
  if(DEFINED ENV{C2A_USER_DIR})
    set(C2A_USER_DIR $ENV{C2A_USER_DIR})
    message(STATUS "C2A user directory from environment: ${C2A_USER_DIR}")
  elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../src_user")
    # レガシー git submodule 方式のフォールバック
    set(C2A_USER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src_user")
    message(STATUS "C2A user directory (legacy fallback): ${C2A_USER_DIR}")
  else()
    message(WARNING "C2A_USER_DIR not set. Build may fail if src_user includes are used.")
  endif()
endif()

set(C2A_SRCS
  c2a_core_main.c
  component_driver/cdrv_common_tlm_cmd_packet.c
  component_driver/cdrv_eb90_frame.c
  component_driver/cdrv_eb90_packet.c
  component_driver/component_driver.c
  component_driver/driver_super.c
  hal/i2c_common.c
  system/application_manager/app_info.c
  system/application_manager/app_manager.c
  system/event_manager/event_manager.c
  system/event_manager/event_logger.c
  system/event_manager/event_handler.c
  system/mode_manager/mode_manager.c
  system/task_manager/task_dispatcher.c
  system/time_manager/obc_time.c
  system/time_manager/time_manager.c
  system/watchdog_timer/watchdog_timer.c
  tlm_cmd/block_command_loader.c
  tlm_cmd/block_command_table.c
  tlm_cmd/block_command_executor.c
  tlm_cmd/command_analyze.c
  tlm_cmd/command_dispatcher.c
  tlm_cmd/command_dispatcher_manager.c
  tlm_cmd/common_packet/common_cmd_packet_util.c
  tlm_cmd/packet_handler.c
  tlm_cmd/packet_list_util.c
  tlm_cmd/packet_list.c
  tlm_cmd/telemetry_frame.c
  tlm_cmd/telemetry_generator.c
  tlm_cmd/ccsds/space_packet_protocol/cmd_space_packet.c
  tlm_cmd/ccsds/space_packet_protocol/space_packet.c
  tlm_cmd/ccsds/space_packet_protocol/tlm_space_packet.c
)

if(C2A_USE_STDINT_WRAPPER)
  message("Use stdint.h wrapper for C89")
  include_directories(library/stdint_wrapper)
endif()

execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_REVISION_C2A_CORE
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_REVISION_C2A_CORE_SHORT
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

# Git revision が取得できない場合のフォールバック（crates.io からのダウンロード時など）
if(NOT GIT_REVISION_C2A_CORE)
  # C2A_CORE_VERSION が指定されている場合は crate version を使用
  if(DEFINED C2A_CORE_VERSION)
    set(GIT_REVISION_C2A_CORE "v${C2A_CORE_VERSION}")
    message(STATUS "Using crate version as revision: v${C2A_CORE_VERSION}")
  else()
    set(GIT_REVISION_C2A_CORE "unknown")
    message(WARNING "Git revision not available (not a git repository)")
  endif()
endif()
if(NOT GIT_REVISION_C2A_CORE_SHORT)
  # C2A_CORE_VERSION が指定されている場合は version から数値を生成
  if(DEFINED C2A_CORE_VERSION)
    # バージョンを 0xMMmmpp 形式に変換 (例: 4.5.1 -> 0x00040501)
    string(REPLACE "." ";" VERSION_LIST ${C2A_CORE_VERSION})
    list(GET VERSION_LIST 0 VERSION_MAJOR)
    list(GET VERSION_LIST 1 VERSION_MINOR)
    list(LENGTH VERSION_LIST VERSION_LIST_LEN)
    if(VERSION_LIST_LEN GREATER 2)
      list(GET VERSION_LIST 2 VERSION_PATCH)
    else()
      set(VERSION_PATCH 0)
    endif()
    math(EXPR VERSION_HEX "(${VERSION_MAJOR} << 16) + (${VERSION_MINOR} << 8) + ${VERSION_PATCH}" OUTPUT_FORMAT HEXADECIMAL)
    # Remove 0x prefix since add_definitions will add it
    string(SUBSTRING "${VERSION_HEX}" 2 -1 VERSION_HEX_NO_PREFIX)
    set(GIT_REVISION_C2A_CORE_SHORT "${VERSION_HEX_NO_PREFIX}")
    message(STATUS "Using crate version as short revision: 0x${VERSION_HEX_NO_PREFIX}")
  else()
    set(GIT_REVISION_C2A_CORE_SHORT "00000000")
    message(WARNING "Git short revision not available (not a git repository)")
  endif()
endif()

add_definitions("-DGIT_REVISION_C2A_CORE=\"${GIT_REVISION_C2A_CORE}\"")
add_definitions("-DGIT_REVISION_C2A_CORE_SHORT=0x${GIT_REVISION_C2A_CORE_SHORT}")

if(C2A_BUILD_AS_CXX)
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

add_library(${PROJECT_NAME} OBJECT ${C2A_SRCS})

# C2A user の親ディレクトリをインクルードパスに追加
# これにより <src_user/...> が解決可能になる（レガシー方式用）
if(DEFINED C2A_USER_DIR AND EXISTS "${C2A_USER_DIR}")
  get_filename_component(C2A_USER_PARENT_DIR "${C2A_USER_DIR}" DIRECTORY)
  target_include_directories(${PROJECT_NAME} PUBLIC "${C2A_USER_PARENT_DIR}")
  message(STATUS "C2A user include path: ${C2A_USER_PARENT_DIR}")
endif()

# ビルドディレクトリ内の src_user シンボリックリンクを解決するため、
# C2A_BUILD_OUT_DIR が指定されている場合は include path に追加
# これにより実際のディレクトリ名を src_user に限定する必要がなくなる
if(DEFINED C2A_BUILD_OUT_DIR)
  target_include_directories(${PROJECT_NAME} PUBLIC "${C2A_BUILD_OUT_DIR}")
  message(STATUS "C2A build output directory: ${C2A_BUILD_OUT_DIR}")
endif()

if(C2A_USE_ALL_CORE_APPS)
  add_subdirectory(applications)
  target_sources(${PROJECT_NAME} PUBLIC $<TARGET_OBJECTS:C2A_CORE_APPS>)
endif()

if(C2A_USE_ALL_CORE_TEST_APPS)
  add_subdirectory(applications/test_app)
  target_sources(${PROJECT_NAME} PUBLIC $<TARGET_OBJECTS:C2A_CORE_TEST_APPS>)
endif()

if(C2A_USE_ALL_CORE_LIB)
  add_subdirectory(library)
  target_sources(${PROJECT_NAME} PUBLIC $<TARGET_OBJECTS:C2A_CORE_LIB>)
endif()

if(C2A_MOBC_FEATURES)
  message("Enable some features for MOBC")
  target_compile_definitions(${PROJECT_NAME} PUBLIC C2A_MOBC_FEATURES)
endif()

if(C2A_USE_SPACE_PACKET_AS_COMMON_PACKET)
  add_subdirectory(tlm_cmd/common_packet/space_packet)
  target_sources(${PROJECT_NAME} PUBLIC $<TARGET_OBJECTS:C2A_CORE_SPACE_PACKET_AS_COMMON_PACKET>)
endif()

if(C2A_USE_CORE_CCSDS_AOS_SPACE_DATA_LINK_PROTOCOL)
  add_subdirectory(tlm_cmd/ccsds/aos_space_data_link_protocol)
  target_sources(${PROJECT_NAME} PUBLIC $<TARGET_OBJECTS:C2A_CORE_CCSDS_AOS_SPACE_DATA_LINK_PROTOCOL>)
endif()

if(C2A_USE_CORE_CCSDS_TC_SPACE_DATA_LINK_PROTOCOL)
  add_subdirectory(tlm_cmd/ccsds/tc_space_data_link_protocol)
  target_sources(${PROJECT_NAME} PUBLIC $<TARGET_OBJECTS:C2A_CORE_CCSDS_TC_SPACE_DATA_LINK_PROTOCOL>)
endif()

include(common.cmake)
