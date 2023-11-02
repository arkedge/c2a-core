#pragma section REPRO
/**
 * @file
 * @brief NOP (No Operation) App
 * @note  C2Aは NOP があることを前提としている
 *        この App は必ず App 登録し， Cmd_NOP も必ずコマンド登録しておくこと
 */
#include "nop.h"
#include <stddef.h>
#include "../tlm_cmd/common_cmd_packet_util.h"
#include "../library/result.h"

static RESULT NOP_nop_(void);

AppInfo NOP_create_app(void)
{
  return AI_create_app_info("nop", NULL, NOP_nop_);
}

static RESULT NOP_nop_(void) {
  // no operation
  return RESULT_OK;
}

CCP_CmdRet Cmd_NOP(const CommonCmdPacket* packet)
{
  (void)packet;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
