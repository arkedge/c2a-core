#pragma section REPRO
#include "tl_elem_debug_display.h"

#include "../../../../applications/app_registry.h"
#include "../../../../tlm_cmd/block_command_definitions.h"

#include <src_core/tlm_cmd/block_command_loader.h>

void BCL_load_tl_elem_debug_display_initial(void)
{
  BCL_tool_register_app    ( 0, AR_APP_DBG_PRINT_GIT_REV);
  BCL_tool_register_app    ( 1, AR_APP_DBG_PRINT_TIMESTAMP);
  BCL_tool_register_app    ( 2, AR_APP_DBG_PRINT_CMD_STATUS);
  BCL_tool_register_app    ( 3, AR_APP_DBG_PRINT_EVENT_LOGGER0);
  BCL_tool_register_app    ( 4, AR_APP_DBG_PRINT_EVENT_LOGGER1);
  BCL_tool_register_app    ( 5, AR_APP_DBG_PRINT_EVENT_HANDLER);
  BCL_tool_register_app    ( 6, AR_NOP);
  BCL_tool_register_app    ( 7, AR_NOP);
  BCL_tool_register_app    ( 8, AR_NOP);
  BCL_tool_register_app    ( 9, AR_APP_DBG_FLUSH_SCREEN);
}

#pragma section
