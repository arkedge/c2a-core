cmake_minimum_required(VERSION 3.13)

project(C2A_CORE_SPACE_PACKET_AS_COMMON_PACKET)

set(C2A_SRCS
  common_cmd_packet.c
  common_tlm_cmd_packet.c
  common_tlm_packet.c
)

if(C2A_BUILD_AS_CXX)
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

add_library(${PROJECT_NAME} OBJECT ${C2A_SRCS})

include(${C2A_CORE_DIR}/common.cmake)
