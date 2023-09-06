cmake_minimum_required(VERSION 3.13)

project(C2A_USER_APPS)

set(C2A_SRCS
  app_registry.c
  driver_instances/di_mobc.c
  user_defined/debug_apps.c
  ${C2A_CORE_DIR}/applications/gs_command_dispatcher.c
  ${C2A_CORE_DIR}/applications/event_utility.c
  ${C2A_CORE_DIR}/applications/nop.c
  ${C2A_CORE_DIR}/applications/realtime_command_dispatcher.c
  ${C2A_CORE_DIR}/applications/timeline_command_dispatcher.c
)

if(C2A_BUILD_AS_CXX)
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

add_library(${PROJECT_NAME} OBJECT ${C2A_SRCS})

include(${C2A_USER_DIR}/common.cmake)
