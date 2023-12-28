#pragma section REPRO
/**
 * @file
 * @brief TL や CDIS などの PacketList や BCT の CCP をダンプする (GS から内容を確認できるようにする) ための App
 */
#include "ccp_dump.h"
#include <stddef.h>
#include <string.h>
#include "../tlm_cmd/command_dispatcher_manager.h"
#include "../system/time_manager/time_manager.h"

/**
 * @brief  App 初期化関数
 * @param  void
 * @return RESULT
 */
static RESULT CCP_DUMP_init_(void);

static RESULT CCP_DUMP_cdis_(const CommandDispatcher* cdis);
static RESULT CCP_DUMP_bct_(void);


static CcpDump ccp_dump_;
const CcpDump* const ccp_dump = &ccp_dump_;


AppInfo CCP_DUMP_create_app(void)
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
  CCP_DUMP_CdisDump* cdis_dump = &ccp_dump_.info.cdis;
  const CommandDispatcher* cdis;
  cdis_dump->cdis_idx  = CCP_get_param_from_packet(packet, 0, uint8_t);
  cdis_dump->queue_idx = CCP_get_param_from_packet(packet, 1, uint16_t);

  cdis = CDIS_MGR_get_cdis(cdis_dump->cdis_idx);

  ccp_dump_.dump.target = CCP_DUMP_TARGET_CDIS;

  if (cdis == NULL)
  {
    ccp_dump_.dump.status = CCP_DUMP_STATUS_NOT_FOUND;
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  }
  if (PL_count_active_nodes(cdis->pl) <= cdis_dump->queue_idx)
  {
    ccp_dump_.dump.status = CCP_DUMP_STATUS_NOT_FOUND;
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);
  }

  if (CCP_DUMP_cdis_(cdis) == RESULT_OK)
  {
    ccp_dump_.dump.status = CCP_DUMP_STATUS_OK;
    ccp_dump_.dump.dump_time = TMGR_get_master_clock();
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  }
  else
  {
    ccp_dump_.dump.status = CCP_DUMP_STATUS_NOT_FOUND;
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 3);
  }
}


static RESULT CCP_DUMP_cdis_(const CommandDispatcher* cdis)
{
  const CCP_DUMP_CdisDump* cdis_dump = &ccp_dump_.info.cdis;
  const PacketList* pl = cdis->pl;
  const PL_Node* node = PL_get_head(pl);
  const CommonCmdPacket* ccp;
  uint8_t i;

  for (i = 0; i < cdis_dump->queue_idx; ++i)
  {
    node = PL_get_next(node);
  }
  if (node == NULL)
  {
    return RESULT_ERR;
  }

  ccp = (const CommonCmdPacket*)PL_get_packet(node);

  // CCP_copy_packet はパケット長しかコピーしない．ここではダンプなので，末端の不定もふくめてコピーする
  memcpy(ccp_dump_.dump.ccp.packet, ccp->packet, CSP_MAX_LEN);
  return RESULT_OK;
}


CCP_CmdRet Cmd_CCP_DUMP_BCT(const CommonCmdPacket* packet)
{
  CCP_DUMP_BctDump* bct_dump = &ccp_dump_.info.bct;
  bct_dump->pos.block = (bct_id_t)CCP_get_param_from_packet(packet, 0, uint16_t);
  bct_dump->pos.cmd = CCP_get_param_from_packet(packet, 1, uint8_t);

  ccp_dump_.dump.target = CCP_DUMP_TARGET_BCT;

  if (BCT_check_position(&bct_dump->pos) != BCT_SUCCESS)
  {
    ccp_dump_.dump.status = CCP_DUMP_STATUS_NOT_FOUND;
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  }

  if (CCP_DUMP_bct_() == RESULT_OK)
  {
    ccp_dump_.dump.status = CCP_DUMP_STATUS_OK;
    ccp_dump_.dump.dump_time = TMGR_get_master_clock();
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  }
  else
  {
    ccp_dump_.dump.status = CCP_DUMP_STATUS_NOT_FOUND;
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);
  }
}


static RESULT CCP_DUMP_bct_(void)
{
  CCP_DUMP_BctDump* bct_dump = &ccp_dump_.info.bct;

  // BCT は CCP 長さが短いので，末尾を 0 埋めしておく
  memset(&ccp_dump_.dump.ccp, 0x00, sizeof(CommonCmdPacket));

  if (BCT_load_cmd(&bct_dump->pos, &ccp_dump_.dump.ccp) == BCT_SUCCESS)
  {
    return RESULT_OK;
  }
  else
  {
    return RESULT_ERR;
  }
}

#pragma section
