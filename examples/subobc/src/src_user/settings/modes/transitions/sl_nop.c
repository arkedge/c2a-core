#pragma section REPRO
#include "sl_nop.h"

#include "../../../tlm_cmd/command_definitions.h"

#include <src_core/tlm_cmd/block_command_loader.h>
#include <src_core/system/time_manager/obc_time.h>

void BCL_load_sl_nop(void)
{
  BCL_tool_register_cmd(OBCT_sec2cycle(5), Cmd_CODE_MM_FINISH_TRANSITION);
}

#pragma section
