cmake_minimum_required(VERSION 3.13)
project(C2A)

# options
# C2A 全体を C99 としてビルドするかどうか
option(C2A_BUILD_AS_C99 "Build C2A as C99" OFF)

# SCI COM for connection to WINGS TMTC IF
# ！！！注意！！！
# これをONにした状態で，SCIの受け口がない場合（TMTC IFが動いてない状態）
# そちらのバッファが詰まってSILSの動作が止まることがあるので注意すること！
option(C2A_USE_SCI_COM_WINGS "Use SCI_COM_WINGS" OFF)

# SCI COM for connection to PC UART
# ！！！注意！！！
# これをONにした状態で，SCIの受け口がない場合（受けてのTeratermが起動していない状態）
# そちらのバッファが詰まってSILSの動作が止まることがあるので注意すること！
option(C2A_USE_SCI_COM_UART "Use SCI_COM_UART" OFF)

option(C2A_BUILD_WITH_SILS_MOCKUP "Build C2A with SILS mockup for check undefined symbols by build minimal C2A user executable(C89 only)" OFF)

option(C2A_BUILD_FOR_SILS "Build C2A for SILS target" ON)

set(C2A_MOBC_FEATURES ON)

# core に同梱されている CCSDS の実装を使う
set(C2A_USE_CORE_CCSDS_AOS_SPACE_DATA_LINK_PROTOCOL ON)
set(C2A_USE_CORE_CCSDS_TC_SPACE_DATA_LINK_PROTOCOL ON)

if(C2A_BUILD_WITH_SILS_MOCKUP)
  set(C2A_BUILD_AS_CXX OFF)
endif()

if(C2A_BUILD_AS_CXX)
  message("Build C2A as C++!")
endif()

# C2A_CORE_DIR と C2A_USER_DIR の設定
# 優先順位: 1. CMake define (-D で指定) 2. レガシーパス（シンボリックリンク）
if(NOT DEFINED C2A_CORE_DIR)
  set(C2A_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/src_core)
  message(STATUS "C2A_CORE_DIR (legacy): ${C2A_CORE_DIR}")
else()
  message(STATUS "C2A_CORE_DIR (defined): ${C2A_CORE_DIR}")
endif()

if(NOT DEFINED C2A_USER_DIR)
  set(C2A_USER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/src_user)
  message(STATUS "C2A_USER_DIR (legacy): ${C2A_USER_DIR}")
else()
  message(STATUS "C2A_USER_DIR (defined): ${C2A_USER_DIR}")
endif()

# C2A user の親ディレクトリをインクルードパスに追加
get_filename_component(C2A_USER_PARENT_DIR "${C2A_USER_DIR}" DIRECTORY)
include_directories("${C2A_USER_PARENT_DIR}")

# C2A core の親ディレクトリもインクルードパスに追加（<src_core/...> 用）
get_filename_component(C2A_CORE_PARENT_DIR "${C2A_CORE_DIR}" DIRECTORY)
if(NOT "${C2A_CORE_PARENT_DIR}" STREQUAL "${C2A_USER_PARENT_DIR}")
  include_directories("${C2A_CORE_PARENT_DIR}")
endif()

# ビルドディレクトリ内の src_core と src_user シンボリックリンクを解決するため、
# C2A_BUILD_OUT_DIR が指定されている場合は include path に追加
# これにより実際のディレクトリ名を src_core/src_user に限定する必要がなくなる
if(DEFINED C2A_BUILD_OUT_DIR)
  include_directories("${C2A_BUILD_OUT_DIR}")
  message(STATUS "C2A build output directory: ${C2A_BUILD_OUT_DIR}")
endif()

# Output debug print to SILS console window
option(C2A_SHOW_DEBUG_PRINT_ON_SILS "Show debug print" ON)
if(C2A_SHOW_DEBUG_PRINT_ON_SILS)
  add_definitions(-DSHOW_DEBUG_PRINT_ON_SILS)
  message("Show debug print")
endif()

execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_REVISION_C2A_USER
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_REVISION_C2A_USER_SHORT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions("-DGIT_REVISION_C2A_USER=\"${GIT_REVISION_C2A_USER}\"")
add_definitions("-DGIT_REVISION_C2A_USER_SHORT=0x${GIT_REVISION_C2A_USER_SHORT}")

# out-of-tree source の場合、binary directory を明示的に指定
add_subdirectory(${C2A_CORE_DIR} ${CMAKE_BINARY_DIR}/c2a_core)

add_subdirectory(${C2A_USER_DIR}/applications)
add_subdirectory(${C2A_USER_DIR}/component_driver)
add_subdirectory(${C2A_USER_DIR}/hal)
add_subdirectory(${C2A_USER_DIR}/library)
add_subdirectory(${C2A_USER_DIR}/settings)
add_subdirectory(${C2A_USER_DIR}/tlm_cmd)

set(C2A_SRCS
  ${C2A_USER_DIR}/c2a_main.c
)

if(C2A_BUILD_AS_CXX)
  message("Build as C++!!!")
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

if(C2A_BUILD_WITH_SILS_MOCKUP)
  add_definitions(-DDEFINE_MAIN_ON_SILS)
  add_executable(${PROJECT_NAME} ${C2A_SRCS})
else()
  add_library(${PROJECT_NAME} STATIC ${C2A_SRCS})
endif()

set(C2A_USER_MODULES
  C2A_USER_APPS
  C2A_USER_CMD_TLM
  C2A_USER_DRIVERS
  C2A_USER_HAL
  C2A_USER_LIB
  C2A_USER_SETTINGS
)

if(MSVC)
  target_link_options(${PROJECT_NAME} PRIVATE "/WHOLEARCHIVE")
  target_link_libraries(${PROJECT_NAME} PRIVATE
    C2A_CORE
    ${C2A_USER_MODULES}
  )
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE
    -Wl,--whole-archive
    C2A_CORE
    -Wl,--no-whole-archive
    ${C2A_USER_MODULES}
    -lm
  )
endif()

include(${C2A_USER_DIR}/common.cmake)
