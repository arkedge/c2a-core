cmake_minimum_required(VERSION 3.13)

project(C2A_USER_DRIVERS)

set(C2A_SRCS
  aocs/aobc_telemetry_buffer.c
  aocs/aobc.c
  com/gs.c
  com/gs_validate.c
  etc/uart_test.c
)

if(C2A_BUILD_AS_CXX)
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

add_library(${PROJECT_NAME} OBJECT ${C2A_SRCS})

include(${C2A_USER_DIR}/common.cmake)
