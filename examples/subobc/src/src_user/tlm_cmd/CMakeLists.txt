cmake_minimum_required(VERSION 3.13)

project(C2A_USER_CMD_TLM)

set(C2A_SRCS
  block_command_definitions.c
  block_command_user_settings.c
  command_definitions.c
  telemetry_definitions.c
  user_packet_handler.c
  normal_block_command_definition/nbc_start_hk_tlm.c
)

if(C2A_BUILD_AS_CXX)
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

add_library(${PROJECT_NAME} OBJECT ${C2A_SRCS})

include(${C2A_USER_DIR}/common.cmake)
