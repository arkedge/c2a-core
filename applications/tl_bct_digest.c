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
#include "../library/crc.h"

/**
 * @brief  App 初期化関数
 * @param  void
 * @return RESULT
 */
static RESULT TL_BCT_DIGEST_init_(void);

/**
 * @brief  CCP の digest を計算
 * @param  ccp: digest を計算する CCP
 * @return digest
 */
static uint16_t TL_BCT_DIGEST_calc_digest_(const CommonCmdPacket* ccp);

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


static uint16_t TL_BCT_DIGEST_calc_digest_(const CommonCmdPacket* ccp)
{
  // 念のため（外部の CCP を扱うので）
  if (!(CCP_is_valid_packet(ccp)))
  {
    return 0;
  }
  return CRC_calc_crc_16_ccitt_left(0xffff, ccp->packet, CCP_get_packet_len(ccp), 0);
}


CCP_CmdRet Cmd_TL_BCT_DIGEST_TL(const CommonCmdPacket* packet)
{
  const PL_Node* node;
  const PacketList* pl;
  uint16_t i;
  TL_BCT_DIGEST_TlDigest* tl_digest = &tl_bct_digest_.tl;
  tl_digest->info.tl_id = (TLCD_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  tl_digest->info.page_no = CCP_get_param_from_packet(packet, 1, uint8_t);

  // 計算結果をクリア
  memset(tl_digest->digests, 0x00, TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE * sizeof(uint16_t));
  tl_digest->info.digests_num = 0;

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
    tl_digest->info.time_stamp = TMGR_get_master_clock();
    // たまたま計算するものがないだけなので， CCP_EXEC_SUCCESS を返す
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  }

  // 計算開始する node まで linked list をすすめる
  node = PL_get_head(pl);

  for (i = 0; i < tl_digest->info.page_no * TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE; ++i)
  {
    node = PL_get_next(node);
  }
  if (node == NULL)
  {
    // ありえないエラー
    tl_digest->info.status = TL_BCT_DIGEST_STATUS_UNKNOWN;
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);
  }

  tl_digest->info.digests_num = tl_digest->info.queued % TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE;
  if (tl_digest->info.queued >= TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE * (tl_digest->info.page_no + 1))
  {
    tl_digest->info.digests_num = TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE;
  }

  for (i = 0; i < tl_digest->info.digests_num; i++)
  {
    if (node == NULL)
    {
      // ありえないエラー
      tl_digest->info.status = TL_BCT_DIGEST_STATUS_UNKNOWN;
      return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 3);
    }

    // digest 計算
    tl_digest->digests[i] = TL_BCT_DIGEST_calc_digest_((const CommonCmdPacket*)PL_get_packet(node));

    node = PL_get_next(node);
  }

  tl_digest->info.status = TL_BCT_DIGEST_STATUS_OK;
  tl_digest->info.time_stamp = TMGR_get_master_clock();
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_TL_BCT_DIGEST_BCT(const CommonCmdPacket* packet)
{
  uint8_t i;
  TL_BCT_DIGEST_BctDigest* bct_digest = &tl_bct_digest_.bct;
  bct_digest->info.block = (bct_id_t)CCP_get_param_from_packet(packet, 0, uint16_t);

  // 計算結果をクリア
  memset(bct_digest->digests, 0x00, BCT_MAX_CMD_NUM * sizeof(uint16_t));
  bct_digest->info.digests_num = 0;

  if (bct_digest->info.block >= BCT_MAX_BLOCKS)
  {
    bct_digest->info.status = TL_BCT_DIGEST_STATUS_PARAM_ERR;
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  }

  bct_digest->info.digests_num = BCT_get_bc_length(bct_digest->info.block);
  for (i = 0; i < bct_digest->info.digests_num; i++)
  {
    const CommonCmdPacket* ccp;
    BCT_Pos pos;
    if (BCT_make_pos(&pos, bct_digest->info.block, i) != BCT_SUCCESS)
    {
      // ありえないエラー
      bct_digest->info.status = TL_BCT_DIGEST_STATUS_UNKNOWN;
      return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);
    }

    // BCT cmd 取得
    // NOTE: const BCT_CmdData → const CommonCmdPacket* への cast
    ccp = (const CommonCmdPacket*)BCT_get_bc_cmd_data(&pos);

    // digest 計算
    bct_digest->digests[i] = TL_BCT_DIGEST_calc_digest_(ccp);
  }

  bct_digest->info.status = TL_BCT_DIGEST_STATUS_OK;
  bct_digest->info.time_stamp = TMGR_get_master_clock();
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
