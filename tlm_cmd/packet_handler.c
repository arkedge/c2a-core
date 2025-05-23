#pragma section REPRO
/**
 * @file
 * @brief C2A 全体を流れる Common Packet の配送を制御する
 */
#include "packet_handler.h"

#include <stddef.h>
#include <string.h>

#include "../system/time_manager/time_manager.h"
#include "../system/event_manager/event_logger.h"
#include "command_analyze.h"
#include "block_command_table.h"
#include <src_user/tlm_cmd/user_packet_handler.h>
#include "packet_list_util.h"

PacketList PH_gs_cmd_list;
PacketList PH_rt_cmd_list;
PacketList PH_tl_cmd_list[TLCD_ID_MAX];
PacketList PH_rt_tlm_list;
#ifdef DR_ENABLE
PacketList PH_st_tlm_list;
PacketList PH_rp_tlm_list;
#endif

static PL_Node PH_gs_cmd_stock_[PH_GSC_LIST_MAX];
static PL_Node PH_rt_cmd_stock_[PH_RTC_LIST_MAX];
static PL_Node PH_tl_cmd_gs_stock_[PH_TLC_GS_LIST_MAX];
static PL_Node PH_tl_cmd_bc_stock_[PH_TLC_BC_LIST_MAX];
static PL_Node PH_tl_cmd_tlm_stock_[PH_TLC_TLM_LIST_MAX];
#ifdef TLCD_ENABLE_MISSION_TL
static PL_Node PH_tl_cmd_mis_stock_[PH_TLC_MIS_LIST_MAX];
#endif
static PL_Node PH_rt_tlm_stock_[PH_RT_TLM_LIST_MAX];
#ifdef DR_ENABLE
static PL_Node PH_st_tlm_stock_[PH_ST_TLM_LIST_MAX];
static PL_Node PH_rp_tlm_stock_[PH_RP_TLM_LIST_MAX];
#endif

static CommonCmdPacket PH_gs_cmd_ccp_stock_[PH_GSC_LIST_MAX];
static CommonCmdPacket PH_rt_cmd_ccp_stock_[PH_RTC_LIST_MAX];
static CommonCmdPacket PH_tl_cmd_gs_ccp_stock_[PH_TLC_GS_LIST_MAX];
static CommonCmdPacket PH_tl_cmd_bc_ccp_stock_[PH_TLC_BC_LIST_MAX];
static CommonCmdPacket PH_tl_cmd_tlm_ccp_stock_[PH_TLC_TLM_LIST_MAX];
#ifdef TLCD_ENABLE_MISSION_TL
static CommonCmdPacket PH_tl_cmd_mis_ccp_stock_[PH_TLC_MIS_LIST_MAX];
#endif
static CommonTlmPacket PH_rt_tlm_ctp_stock_[PH_RT_TLM_LIST_MAX];
#ifdef DR_ENABLE
static CommonTlmPacket PH_st_tlm_ctp_stock_[PH_ST_TLM_LIST_MAX];
static CommonTlmPacket PH_rp_tlm_ctp_stock_[PH_RP_TLM_LIST_MAX];
#endif

static PH_ACK PH_add_block_cmd_(const CommonCmdPacket* packet);
static PH_ACK PH_add_gs_cmd_(const CommonCmdPacket* packet);
static PH_ACK PH_add_rt_cmd_(const CommonCmdPacket* packet);
static PH_ACK PH_add_tl_cmd_(TLCD_ID id,
                             const CommonCmdPacket* packet,
                             cycle_t now);
/**
 * @brief UTL_cmd を TL_cmd に変換して tl_cmd_list に追加する
 * @note TODO：const cast でもいいか検討する
 * @param[in] packet
 * @return PH_ACK
 */
static PH_ACK PH_add_utl_cmd_(TLCD_ID id, const CommonCmdPacket* packet);
static PH_ACK PH_add_tlm_to_pl(const CommonTlmPacket* packet, PacketList* pl, CTP_DEST_FLAG dest_flag);
static PH_ACK PH_add_rt_tlm_(const CommonTlmPacket* packet);
#ifdef DR_ENABLE
static PH_ACK PH_add_st_tlm_(const CommonTlmPacket* packet);
static PH_ACK PH_add_rp_tlm_(const CommonTlmPacket* packet);
#endif


void PH_init(void)
{
  PL_initialize_with_ccp(PH_gs_cmd_stock_, PH_gs_cmd_ccp_stock_, PH_GSC_LIST_MAX, &PH_gs_cmd_list);
  PL_initialize_with_ccp(PH_rt_cmd_stock_, PH_rt_cmd_ccp_stock_, PH_RTC_LIST_MAX, &PH_rt_cmd_list);

  PL_initialize_with_ccp(PH_tl_cmd_gs_stock_, PH_tl_cmd_gs_ccp_stock_, PH_TLC_GS_LIST_MAX, &PH_tl_cmd_list[TLCD_ID_FROM_GS]);
  PL_initialize_with_ccp(PH_tl_cmd_bc_stock_, PH_tl_cmd_bc_ccp_stock_, PH_TLC_BC_LIST_MAX, &PH_tl_cmd_list[TLCD_ID_DEPLOY_BC]);
  PL_initialize_with_ccp(PH_tl_cmd_tlm_stock_, PH_tl_cmd_tlm_ccp_stock_, PH_TLC_TLM_LIST_MAX, &PH_tl_cmd_list[TLCD_ID_DEPLOY_TLM]);
#ifdef TLCD_ENABLE_MISSION_TL
  PL_initialize_with_ccp(PH_tl_cmd_mis_stock_, PH_tl_cmd_mis_ccp_stock_, PH_TLC_TLM_LIST_MAX, &PH_tl_cmd_list[TLCD_ID_FROM_GS_FOR_MISSION]);
#endif

  PL_initialize_with_ctp(PH_rt_tlm_stock_, PH_rt_tlm_ctp_stock_, PH_RT_TLM_LIST_MAX, &PH_rt_tlm_list);
#ifdef DR_ENABLE
  PL_initialize_with_ctp(PH_st_tlm_stock_, PH_st_tlm_ctp_stock_, PH_ST_TLM_LIST_MAX, &PH_st_tlm_list);
  PL_initialize_with_ctp(PH_rp_tlm_stock_, PH_rp_tlm_ctp_stock_, PH_RP_TLM_LIST_MAX, &PH_rp_tlm_list);
#endif

  PH_user_init();
}


PH_ACK PH_analyze_packet(const CommonTlmCmdPacket* packet)
{
  if (!CTCP_is_valid_packet(packet)) return PH_ACK_INVALID_PACKET;

  switch (CTCP_get_packet_type(packet))
  {
  case CTCP_PACKET_TYPE_TLM:
    // CTP 変換の NULL チェックは PH_analyze_tlm_packet 内で
    return PH_analyze_tlm_packet(CTCP_convert_to_ctp(packet));

  case CTCP_PACKET_TYPE_CMD:
    // CCP 変換の NULL チェックは PH_analyze_cmd_packet 内で
    return PH_analyze_cmd_packet(CTCP_convert_to_ccp(packet));

  default:
    return PH_ACK_INVALID_PACKET;
  }

  return PH_ACK_UNKNOWN;
}


PH_ACK PH_analyze_cmd_packet(const CommonCmdPacket* packet)
{
  PH_ACK ack;
  uint32_t el_local;

  if (!CCP_is_valid_packet(packet)) return PH_ACK_INVALID_PACKET;

  // ユーザー定義部
  // 基本的には，接続されているC2Aを搭載したボードに
  // この段階（キューイングされ，時刻調整され， PH_user_cmd_router で実行されるのではなく，この段階）で転送したいときに使う
  ack = PH_user_analyze_cmd(packet);
  if (ack != PH_ACK_UNKNOWN)
  {
    if (ack == PH_ACK_FORWARDED)
    {
      // sub OBC キューへ無事に転送
      return ack;
    }

    // 転送失敗
    el_local = ( ((0X000000ff & (uint32_t)CCP_get_dest_type(packet)) << 24)
               | ((0x000000ff & (uint32_t)CCP_get_exec_type(packet)) << 16)
               | ( 0x0000ffff & (uint32_t)ack) );
    EL_record_event((EL_GROUP)EL_CORE_GROUP_PH_USER_ANALYZE_CCP,
                    el_local,
                    EL_ERROR_LEVEL_HIGH,
                    CCP_get_id(packet));
    return ack;
  }

  // ここまで来たら自分宛て
  // 例えば以下のどれか
  // - CCP_DEST_TYPE_TO_ME
  // - CCP_DEST_TYPE_TO_APID でかつ， APID が自分宛てのもの
  // - CCP_DEST_TYPE_TO_MOBC などの自分宛
  // 統一するため上書きする
  CCP_set_dest_type((CommonCmdPacket*)packet, CCP_DEST_TYPE_TO_ME);   // const_cast

  switch (CCP_get_exec_type(packet))
  {
  case CCP_EXEC_TYPE_GS:
    ack = PH_add_gs_cmd_(packet);
    break;

  case CCP_EXEC_TYPE_TL_FROM_GS:
    ack = PH_add_tl_cmd_(TLCD_ID_FROM_GS, packet, TMGR_get_master_total_cycle());
    break;

  case CCP_EXEC_TYPE_BC:
    ack = PH_add_block_cmd_(packet);
    break;

  case CCP_EXEC_TYPE_RT:
    ack = PH_add_rt_cmd_(packet);
    break;

  case CCP_EXEC_TYPE_UTL:
    ack = PH_add_utl_cmd_(TLCD_ID_FROM_GS, packet);
    break;

  case CCP_EXEC_TYPE_TL_DEPLOY_BC:
    ack = PH_add_tl_cmd_(TLCD_ID_DEPLOY_BC, packet, TMGR_get_master_total_cycle());
    break;

  case CCP_EXEC_TYPE_TL_DEPLOY_TLM:
    ack = PH_add_tl_cmd_(TLCD_ID_DEPLOY_TLM, packet, TMGR_get_master_total_cycle());
    break;

#ifdef TLCD_ENABLE_MISSION_TL
  case CCP_EXEC_TYPE_TL_FOR_MISSION:
    ack = PH_add_tl_cmd_(TLCD_ID_FROM_GS_FOR_MISSION, packet, TMGR_get_master_total_cycle());
    break;

  case CCP_EXEC_TYPE_UTL_FOR_MISSION:
    ack = PH_add_utl_cmd_(TLCD_ID_FROM_GS_FOR_MISSION, packet);
    break;
#endif

  default:
    ack = PH_ACK_UNKNOWN;
    break;
  }

  switch (ack)
  {
  case PH_ACK_SUCCESS:
  case PH_ACK_FORWARDED:
  case PH_ACK_TLC_SUCCESS:
  case PH_ACK_BC_SUCCESS:
    // 成功ケース
    return ack;
  default:
    break;
  }

  // ここまで来たら以下のどれか
  // 全てにおいてパケットが失われているので， EL_ERROR_LEVEL_HIGH として対応
  // PH_ACK_PL_LIST_FULL          // キューに空きがなく，パケットが失われるので HIGH
  // PH_ACK_PACKET_NOT_FOUND      // これがくることはないはず
  // PH_ACK_INVALID_PACKET        // これがくることはないはず
  // PH_ACK_TLC_PAST_TIME         // 設定時刻が不正でパケットが失われるので HIGH
  // PH_ACK_TLC_ALREADY_EXISTS    // 設定時刻が不正でパケットが失われるので HIGH
  // PH_ACK_BC_INVALID_BLOCK_NO   // BC ID が不正でパケットが失われるので HIGH
  // PH_ACK_BC_ISORATED_CMD       // BC Cmd pos が不正でパケットが失われるので HIGH
  // PH_ACK_BC_CMD_TOO_LONG       // BC Cmd param 長 が不正でパケットが失われるので HIGH
  // PH_ACK_UNKNOWN               // これがくることはないはず
  el_local = ( ((0X000000ff & (uint32_t)CCP_get_dest_type(packet)) << 24)     // ここでの CCP_DEST_TYPE は CCP_DEST_TYPE_TO_ME のはず
             | ((0x000000ff & (uint32_t)CCP_get_exec_type(packet)) << 16)
             | ( 0x0000ffff & (uint32_t)ack) );
  EL_record_event((EL_GROUP)EL_CORE_GROUP_PH_ANALYZE_CCP,
                  el_local,
                  EL_ERROR_LEVEL_HIGH,
                  CCP_get_id(packet));

  return ack;
}


static PH_ACK PH_add_block_cmd_(const CommonCmdPacket* packet)
{
  switch (BCT_register_cmd(packet))
  {
  case BCT_SUCCESS:
    return PH_ACK_BC_SUCCESS;

  case BCT_INVALID_BLOCK_NO:
    return PH_ACK_BC_INVALID_BLOCK_NO;

  case BCT_INVALID_CMD_NO:
    return PH_ACK_INVALID_PACKET;

  case BCT_ISORATED_CMD:
    return PH_ACK_BC_ISORATED_CMD;

  case BCT_CMD_TOO_LONG:
    return PH_ACK_BC_CMD_TOO_LONG;

  default:
    return PH_ACK_UNKNOWN;
  }
}


PH_ACK PH_analyze_tlm_packet(const CommonTlmPacket* packet)
{
  ctp_dest_flags_t flags;
  PH_ACK ack = PH_ACK_UNKNOWN;
  PH_ACK last_err_ack = PH_ACK_SUCCESS;

  if (!CTP_is_valid_packet(packet)) return PH_ACK_INVALID_PACKET;

  flags = CTP_get_dest_flags(packet);

  // FIXME: flag の match は関数化したい

  // High Priority Realtime Telemetry
  if (flags & CTP_DEST_FLAG_HP_TLM)
  {
    ack = PH_add_rt_tlm_(packet);  // hp_tlm のフラグが立っていても，RT_TLM として処理する方針にした
    if (ack != PH_ACK_SUCCESS) last_err_ack = ack;
  }

  // Realtime Telemetry
  if (flags & CTP_DEST_FLAG_RT_TLM)
  {
    ack = PH_add_rt_tlm_(packet);
    if (ack != PH_ACK_SUCCESS) last_err_ack = ack;
  }

#ifdef DR_ENABLE
  // Stored Telemetry
  if (flags & CTP_DEST_FLAG_ST_TLM)
  {
    ack = PH_add_st_tlm_(packet);
    if (ack != PH_ACK_SUCCESS) last_err_ack = ack;
  }

  // Replay Telemetry
  if (flags & CTP_DEST_FLAG_RP_TLM)
  {
    ack = PH_add_rp_tlm_(packet);
    if (ack != PH_ACK_SUCCESS) last_err_ack = ack;
  }
#endif

  if (ack == PH_ACK_UNKNOWN)
  {
    // どの CTP_DEST_FLAG にもヒットしなかった場合
    last_err_ack = PH_ACK_UNKNOWN;
    EL_record_event((EL_GROUP)EL_CORE_GROUP_PH_ANALYZE_CTP,
                    (uint32_t)( ((0X000000ff & (uint32_t)flags) << 16) | ( 0x0000ffff & (uint32_t)last_err_ack) ),
                    EL_ERROR_LEVEL_LOW,
                    CTP_get_id(packet));
  }

  return last_err_ack;
}


CCP_CmdRet PH_dispatch_command(const CommonCmdPacket* packet)
{
  if (!CCP_is_valid_packet(packet))
  {
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_UNKNOWN);    // FIXME: 返り値変えたい
  }

  // FIXME: CTCP, SpacePacket 整理で直す
  if (CCP_get_apid(packet) == CCP_APID_CMD_TO_ME)
  {
    // 自分宛てのコマンドの場合は対応処理を呼び出し。
    return CA_execute_cmd(packet);
  }
  else
  {
    // 別機器宛コマンドの場合はパケット分配処理へ
    return PH_user_cmd_router(packet);
  }
}


static PH_ACK PH_add_gs_cmd_(const CommonCmdPacket* packet)
{
  PL_ACK ack = PL_push_back(&PH_gs_cmd_list, packet);

  if (ack != PL_SUCCESS) return PH_ACK_PL_LIST_FULL;

  return PH_ACK_SUCCESS;
}


static PH_ACK PH_add_rt_cmd_(const CommonCmdPacket* packet)
{
  PL_ACK ack = PL_push_back(&PH_rt_cmd_list, packet);

  if (ack != PL_SUCCESS) return PH_ACK_PL_LIST_FULL;

  return PH_ACK_SUCCESS;
}


static PH_ACK PH_add_tl_cmd_(TLCD_ID id,
                            const CommonCmdPacket* packet,
                            cycle_t now)
{
  PL_ACK ack = PL_insert_tl_cmd(&(PH_tl_cmd_list[id]), packet, now);

  switch (ack)
  {
  case PL_SUCCESS:
    return PH_ACK_TLC_SUCCESS;

  case PL_LIST_FULL:
    return PH_ACK_PL_LIST_FULL;

  case PL_TLC_PAST_TIME:
    return PH_ACK_TLC_PAST_TIME;

  case PL_TLC_ALREADY_EXISTS:
    return PH_ACK_TLC_ALREADY_EXISTS;

  default:
    return PH_ACK_UNKNOWN;
  }
}


static PH_ACK PH_add_utl_cmd_(TLCD_ID id, const CommonCmdPacket* packet)
{
  CommonCmdPacket temp;

  // utl_unixtime : time_manager.h の utl_unixtime_epoch_ を参照
  // UTL_cmd ではパケットヘッダーの ti の部分に utl_unixtime が格納されている
  cycle_t utl_unixtime = CCP_get_ti(packet);
  cycle_t ti = TMGR_get_ti_from_utl_unixtime(utl_unixtime);

  // TL_cmd に変換して tl_cmd_list に追加する
  CCP_copy_packet(&temp, packet);
  CCP_set_ti(&temp, ti);
  CCP_set_exec_type(&temp, CCP_EXEC_TYPE_TL_FROM_GS); // UTL -> TL

  return PH_add_tl_cmd_(id, &temp, TMGR_get_master_total_cycle());
}


static PH_ACK PH_add_tlm_to_pl(const CommonTlmPacket* packet, PacketList* pl, CTP_DEST_FLAG dest_flag)
{
  PL_Node* tail;
  PL_ACK ack = PL_push_back(pl, packet);

  if (ack != PL_SUCCESS)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_PH_ANALYZE_CTP,
                    (uint32_t)( ((0X000000ff & (uint32_t)dest_flag) << 16) | ( 0x0000ffff & (uint32_t)PH_ACK_PL_LIST_FULL) ),
                    EL_ERROR_LEVEL_HIGH,      // packet が失われるので HIGH
                    CTP_get_id(packet));
    return PH_ACK_PL_LIST_FULL;
  }

  // 複数の配送先に配送されるパケットの分岐は終わっているため， dest flag を配送先のもののみにする．
  // こうすることで， GS SW 側でのデータベース格納の処理がシンプルになる．
  // PL_SUCCESS の場合，かならず tail に格納した packet がある．
  // TODO: メモリコピーをなくすためにだいぶやんちゃな実装なので，ぱっといい方法が思いつくなら直す
  tail = (PL_Node*)PL_get_tail(pl);      // const_cast
  CTP_set_dest_flags((CommonTlmPacket*)(tail->packet), (ctp_dest_flags_t)dest_flag);

  return PH_ACK_SUCCESS;
}


static PH_ACK PH_add_rt_tlm_(const CommonTlmPacket* packet)
{
  return PH_add_tlm_to_pl(packet, &PH_rt_tlm_list, CTP_DEST_FLAG_RT_TLM);
}


#ifdef DR_ENABLE
static PH_ACK PH_add_st_tlm_(const CommonTlmPacket* packet)
{
  return PH_add_tlm_to_pl(packet, &PH_st_tlm_list, CTP_DEST_FLAG_ST_TLM);
}
#endif


#ifdef DR_ENABLE
static PH_ACK PH_add_rp_tlm_(const CommonTlmPacket* packet)
{
  return PH_add_tlm_to_pl(packet, &PH_rp_tlm_list, CTP_DEST_FLAG_RP_TLM);
}
#endif

const PacketList* PH_get_packet_list_from_exec_type(CCP_EXEC_TYPE type)
{
  switch (type)
  {
  case CCP_EXEC_TYPE_GS:
    return &PH_gs_cmd_list;

  case CCP_EXEC_TYPE_TL_FROM_GS:
  case CCP_EXEC_TYPE_UTL:
    return &PH_tl_cmd_list[TLCD_ID_FROM_GS];

  case CCP_EXEC_TYPE_RT:
    return &PH_rt_cmd_list;

  case CCP_EXEC_TYPE_TL_DEPLOY_BC:
    return &PH_tl_cmd_list[TLCD_ID_DEPLOY_BC];

  case CCP_EXEC_TYPE_TL_DEPLOY_TLM:
    return &PH_tl_cmd_list[TLCD_ID_DEPLOY_TLM];

#ifdef TLCD_ENABLE_MISSION_TL
  case CCP_EXEC_TYPE_TL_FOR_MISSION:
  case CCP_EXEC_TYPE_UTL_FOR_MISSION:
    return &PH_tl_cmd_list[TLCD_ID_FROM_GS_FOR_MISSION];
#endif

  default:
    break;
  }

  return NULL;
}

#pragma section
