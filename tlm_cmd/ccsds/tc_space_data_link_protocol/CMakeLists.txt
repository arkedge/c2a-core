cmake_minimum_required(VERSION 3.13)

project(C2A_CORE_CCSDS_TC_SPACE_DATA_LINK_PROTOCOL)

set(C2A_SRCS
  tc_segment.c
  tc_transfer_frame.c
)

if(C2A_BUILD_AS_CXX)
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

add_library(${PROJECT_NAME} OBJECT ${C2A_SRCS})

include(${C2A_CORE_DIR}/common.cmake)
