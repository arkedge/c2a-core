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

option(C2A_USE_SIMPLE_LIBC          "Use C2A-core hosted simple libc implementation" OFF)

set(C2A_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(C2A_SRCS
  c2a_core_main.c
  Drivers/Protocol/common_tlm_cmd_packet_for_driver_super.c
  Drivers/Protocol/eb90_frame_for_driver_super.c
  Drivers/Protocol/eb90_packet_for_driver_super.c
  Drivers/Super/driver_super.c
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
  tlm_cmd/common_cmd_packet_util.c
  tlm_cmd/packet_handler.c
  tlm_cmd/packet_list_util.c
  tlm_cmd/packet_list.c
  tlm_cmd/telemetry_frame.c
  tlm_cmd/telemetry_generator.c
  tlm_cmd/ccsds/cmd_space_packet.c
  tlm_cmd/ccsds/space_packet.c
  tlm_cmd/ccsds/tlm_space_packet.c
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

include(common.cmake)
