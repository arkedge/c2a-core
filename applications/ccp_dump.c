#pragma section REPRO
/**
 * @file
 * @brief TL や CDIS などの PacketList や BCT の CCP をダンプする (GS から内容を確認できるようにする) ための App
 */
#include "ccp_dump.h"
#include <stddef.h>

/**
 * @brief  App 初期化関数
 * @param  void
 * @return RESULT
 */
static RESULT CCP_DUMP_init_(void);

static CcpDump ccp_dump_;
const CcpDump* const ccp_dump = &ccp_dump_;


AppInfo MEM_create_app(void)
{
  return AI_create_app_info("ccpdump", CCP_DUMP_init_, NULL);
}


static RESULT CCP_DUMP_init_(void)
{
  memset(&ccp_dump_, 0x00, sizeof(CcpDump));

  // 初回はなにもダンプしてないので
  ccp_dump_.dump.status = CCP_DUMP_STATUS_NOT_FOUND;
  return RESULT_OK;
}


CCP_CmdRet Cmd_CCP_DUMP_CDIS(const CommonCmdPacket* packet)
{
  ccp_dump_.info.cdis.cdis_idx  = CCP_get_param_from_packet(packet, 0, uint8_t);
  ccp_dump_.info.cdis.queue_idx = CCP_get_param_from_packet(packet, 1, uint16_t);


  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CCP_DUMP_BCT(const CommonCmdPacket* packet)
{
  ccp_dump_.info.bct.pos.block = (bct_id_t)CCP_get_param_from_packet(packet, 0, uint16_t);
  ccp_dump_.info.bct.pos.cmd = CCP_get_param_from_packet(packet, 1, uint8_t);


  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
