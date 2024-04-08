cmake_minimum_required(VERSION 3.13)

project(C2A_USER_SETTINGS)

set(C2A_SRCS
  component_driver/component_driver_define.c
  component_driver/hal_handler_registry.c
  modes/mode_definitions.c
  modes/task_lists/tl_initial.c
  modes/task_lists/elements/tl_elem_debug_display.c
  modes/task_lists/elements/tl_elem_gs_related_process.c
  modes/task_lists/elements/tl_elem_tlm_cmd_hirate.c
  modes/transitions/sl_initial.c
  modes/transitions/sl_nop.c
  system/event_logger_settings.c
  system/event_handler_rules/event_handler_rules.c
  tlm_cmd/common_cmd_packet_define.c
  tlm_cmd/common_tlm_packet_define.c
  tlm_cmd/ccsds/apid_define.c
)

if(C2A_BUILD_AS_CXX)
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

add_library(${PROJECT_NAME} OBJECT ${C2A_SRCS})

include(${C2A_USER_DIR}/common.cmake)
