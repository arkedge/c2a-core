#pragma section REPRO
/**
 * @file
 * @brief  packet_handlerのコマンドルーター，コマンドアナライザのユーザー定義部分
 */
#include "user_packet_handler.h"
#include <src_core/TlmCmd/packet_list_util.h>
#include "../Applications/DriverInstances/di_aobc.h"
// #include "../Applications/DriverInstances/di_tobc.h"

#define PH_AOBC_CMD_LIST_MAX   (16)          //!< AOBC CMD QUEUEサイズ
#define PH_TOBC_CMD_LIST_MAX   (16)          //!< TOBC CMD QUEUEサイズ

static PH_ACK PH_add_aobc_cmd_(const CommonCmdPacket* packet);
static PH_ACK PH_add_tobc_cmd_(const CommonCmdPacket* packet);

PacketList PH_aobc_cmd_list;
PacketList PH_tobc_cmd_list;

static PL_Node PH_aobc_cmd_stock_[PH_AOBC_CMD_LIST_MAX];
static PL_Node PH_tobc_cmd_stock_[PH_TOBC_CMD_LIST_MAX];
static CommonCmdPacket PH_aobc_ccp_stock_[PH_AOBC_CMD_LIST_MAX];
static CommonCmdPacket PH_tobc_ccp_stock_[PH_TOBC_CMD_LIST_MAX];


void PH_user_init(void)
{
  PL_initialize_with_ccp(PH_aobc_cmd_stock_, PH_aobc_ccp_stock_, PH_AOBC_CMD_LIST_MAX, &PH_aobc_cmd_list);
  PL_initialize_with_ccp(PH_tobc_cmd_stock_, PH_tobc_ccp_stock_, PH_TOBC_CMD_LIST_MAX, &PH_tobc_cmd_list);
}


PH_ACK PH_user_analyze_cmd(const CommonCmdPacket* packet)
{
  switch (CCP_get_dest_type(packet))
  {
  case CCP_DEST_TYPE_TO_AOBC:
    return (PH_add_aobc_cmd_(packet) == PH_ACK_SUCCESS) ? PH_ACK_FORWARDED : PH_ACK_PL_LIST_FULL;
  case CCP_DEST_TYPE_TO_TOBC:
    return (PH_add_tobc_cmd_(packet) == PH_ACK_SUCCESS) ? PH_ACK_FORWARDED : PH_ACK_PL_LIST_FULL;
  default:
    // CCP_DEST_TYPE_TO_ME
    // CCP_DEST_TYPE_TO_MOBC （自分）
    // 宛先不明
    // はここに
    return PH_ACK_UNKNOWN;
  }
}


CCP_CmdRet PH_user_cmd_router(const CommonCmdPacket* packet)
{
  APID apid = CCP_get_apid(packet);
  switch (apid)
  {
  case APID_AOBC_CMD:
    // AOBCに配送
    return DI_AOBC_dispatch_command(packet);
  case APID_TOBC_CMD:
    // TOBCに配送
    // return DI_TOBC_dispatch_command(packet);
  default:
    // 該当する配送先が定義されていない場合
    return CCP_make_cmd_ret(CCP_EXEC_ROUTING_FAILED, apid);
  }
}


static PH_ACK PH_add_aobc_cmd_(const CommonCmdPacket* packet)
{
  PL_ACK ack = PL_push_back(&PH_aobc_cmd_list, packet);

  if (ack != PL_SUCCESS) return PH_ACK_PL_LIST_FULL;
  return PH_ACK_SUCCESS;
}


static PH_ACK PH_add_tobc_cmd_(const CommonCmdPacket* packet)
{
  (void)packet;
#if 0
  PL_ACK ack = PL_push_back(&PH_tobc_cmd_list, packet);

  if (ack != PL_SUCCESS) return PH_ACK_PL_LIST_FULL;
#endif
  return PH_ACK_SUCCESS;
}

#pragma section
