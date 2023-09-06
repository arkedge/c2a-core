cmake_minimum_required(VERSION 3.13)

project(C2A_CORE)

# Build option
## C2A compile config
option(C2A_BUILD_AS_UTF8            "Build C2A as UTF-8" ON)

# user config
option(USE_ALL_C2A_CORE_APPS        "use C2A-core all Applications" ON)
option(USE_ALL_C2A_CORE_TEST_APPS   "use C2A-core all Test Applications" ON)
option(USE_ALL_C2A_CORE_LIB         "use C2A-core all library" ON)
option(USE_32BIT_COMPILER           "use 32bit compiler" OFF)

option(C2A_USE_SIMPLE_LIBC          "use C2A-core hosted simple libc implementation" OFF)
option(C2A_USE_C99_STDINT           "use C99 standard stdint.h" ON)

option(BUILD_C2A_AS_SILS_FW         "build C2A as SILS firmware" ON)
option(BUILD_C2A_AS_C99             "build C2A as C99" OFF)
option(BUILD_C2A_AS_CXX             "build C2A as C++" OFF)

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

if(NOT C2A_USE_C99_STDINT)
  message("Use stdint.h wrapper")
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

if(BUILD_C2A_AS_CXX)
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

add_library(${PROJECT_NAME} OBJECT ${C2A_SRCS})

if(USE_ALL_C2A_CORE_APPS)
  add_subdirectory(applications)
  target_sources(${PROJECT_NAME} PUBLIC $<TARGET_OBJECTS:C2A_CORE_APPS>)
endif()

if(USE_ALL_C2A_CORE_TEST_APPS)
  add_subdirectory(applications/test_app)
  target_sources(${PROJECT_NAME} PUBLIC $<TARGET_OBJECTS:C2A_CORE_TEST_APPS>)
endif()

if(USE_ALL_C2A_CORE_LIB)
  add_subdirectory(library)
  target_sources(${PROJECT_NAME} PUBLIC $<TARGET_OBJECTS:C2A_CORE_LIB>)
endif()

include(common.cmake)
