#pragma section REPRO
#include "tl_elem_gs_related_process.h"

#include "../../../../tlm_cmd/block_command_definitions.h"
#include "../../../../applications/app_registry.h"

#include <src_core/tlm_cmd/block_command_loader.h>

void BCL_load_tl_elem_gs_related_process(void)
{
  BCL_tool_register_app(0, AR_CSRV_MOBC_CMD_PH);
  BCL_tool_register_app(1, AR_GSC_DISPATCHER);
}

#pragma section
