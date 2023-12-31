#pragma section REPRO
/**
 * @file
 * @brief TL や BCT のコマンド列の digest を計算し， GS と FSW での整合性を確認するための App
 */
#include "tl_bct_digest.h"
#include <stddef.h>
#include <string.h>
#include "../tlm_cmd/command_dispatcher_manager.h"
#include "../system/time_manager/time_manager.h"

/**
 * @brief  App 初期化関数
 * @param  void
 * @return RESULT
 */
static RESULT TL_BCT_DIGEST_init_(void);

static RESULT TL_BCT_DIGEST_cdis_(const CommandDispatcher* cdis);
static RESULT TL_BCT_DIGEST_bct_(void);


static TlBctDigest tl_bct_digest_;
const TlBctDigest* const tl_bct_digest = &tl_bct_digest_;


AppInfo TL_BCT_DIGEST_create_app(void)
{
  return AI_create_app_info("tl_bct_digest", TL_BCT_DIGEST_init_, NULL);
}


static RESULT TL_BCT_DIGEST_init_(void)
{
  memset(&tl_bct_digest_, 0x00, sizeof(TlBctDigest));
  return RESULT_OK;
}


CCP_CmdRet Cmd_TL_BCT_DIGEST_TL(const CommonCmdPacket* packet)
{
  const PL_Node* node_head;
  const PL_Node* node;
  const PacketList* pl;
  uint16_t i;
  TL_BCT_DIGEST_TlDigest* tl_digest = &tl_bct_digest_.tl;
  tl_digest->info.tl_id = (TLCD_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  tl_digest->info.page_no = CCP_get_param_from_packet(packet, 1, uint8_t);

  // 計算結果をクリア
  memset(tl_digest->digests, 0x00, TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE * sizeof(uint16_t));

  if (tl_digest->info.tl_id >= TLCD_ID_MAX)
  {
    tl_digest->info.status = TL_BCT_DIGEST_STATUS_PARAM_ERR;
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  }

  pl = &(PH_tl_cmd_list[tl_digest->info.tl_id]);
  tl_digest->info.queued = PL_count_active_nodes(pl);

  if (tl_digest->info.queued <= tl_digest->info.page_no * TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE)
  {
    tl_digest->info.status = TL_BCT_DIGEST_STATUS_NO_CCP;
    tl_digest->info.digests_num = 0;
    tl_digest->info.time_stamp = TMGR_get_master_clock();
    // たまたま計算するものがないだけなので， CCP_EXEC_SUCCESS を返す
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  }

  // 計算開始する node まで linked list をすすめる
  node_head = PL_get_head(pl);

  // for (i = 0; i < cdis_dump->queue_idx; ++i)
  // {
  //   node = PL_get_next(node);
  // }
  // if (node == NULL)
  // {
  //   return RESULT_ERR;
  // }


  // TODO


  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_TL_BCT_DIGEST_BCT(const CommonCmdPacket* packet)
{
  TL_BCT_DIGEST_BctDigest* bct_digest = &tl_bct_digest_.bct;
  bct_digest->info.block = (bct_id_t)CCP_get_param_from_packet(packet, 0, uint16_t);

  // 計算結果をクリア
  memset(bct_digest->digests, 0x00, BCT_MAX_CMD_NUM * sizeof(uint16_t));

  if (bct_digest->info.block >= BCT_MAX_BLOCKS)
  {
    bct_digest->info.status = TL_BCT_DIGEST_STATUS_PARAM_ERR;
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  }

  bct_digest->info.digests_num = BCT_get_bc_length(bct_digest->info.block);


  // TODO


  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
