cmake_minimum_required(VERSION 3.13)

project(C2A_CORE)

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
# See alse; /docs/core/ccsds.md
option(C2A_USE_CORE_CCSDS_AOS_SPACE_DATA_LINK_PROTOCOL  "Use C2A-core CCSDS AOS SPACE DATA LINK PROTOCOL implementation" OFF)
option(C2A_USE_CORE_CCSDS_TC_SPACE_DATA_LINK_PROTOCOL   "Use C2A-core CCSDS TC SPACE DATA LINK PROTOCOL implementation" OFF)

option(C2A_USE_SIMPLE_LIBC          "Use C2A-core hosted simple libc (c2a-core/library/libc) implementation" OFF)

set(C2A_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

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
)
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_REVISION_C2A_CORE_SHORT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions("-DGIT_REVISION_C2A_CORE=\"${GIT_REVISION_C2A_CORE}\"")
add_definitions("-DGIT_REVISION_C2A_CORE_SHORT=0x${GIT_REVISION_C2A_CORE_SHORT}")

if(C2A_BUILD_AS_CXX)
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

add_library(${PROJECT_NAME} OBJECT ${C2A_SRCS})

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
