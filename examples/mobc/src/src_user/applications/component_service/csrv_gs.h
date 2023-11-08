/**
 * @file
 * @brief GS Driver のインスタンス化
 */
#ifndef CSRV_GS_H_
#define CSRV_GS_H_

#include <stdint.h>

#include "../../component_driver/com/gs.h"
#include "../../tlm_cmd/ccsds/tcp_to_m_pdu.h"
#include "../../tlm_cmd/ccsds/aos_transfer_frame.h"
#include <src_core/system/application_manager/app_info.h>

/**
 * @struct CSRV_GS_TlmPacketHandler
 * @brief  テレメの送信
 */
typedef struct
{
  TcpToMPdu tc_packet_to_m_pdu; //!< PL から取り出した tc_packet を M_PDU に変換する
  AOSTF aostf;              //!< 送信するAOSTF
  uint32_t aostf_counter;  //!< AOSTF counter
} CSRV_GS_TlmPacketHandler;

extern const GS_Driver* const gs_driver;
extern const CSRV_GS_TlmPacketHandler* const CSRV_GS_rt_tlm_packet_handler;
extern const CSRV_GS_TlmPacketHandler* const CSRV_GS_rp_tlm_packet_handler;

// 以下インスタンス化
AppInfo CSRV_GS_cmd_packet_handler_app(void);

AppInfo CSRV_GS_rt_tlm_packet_handler_app(void);
AppInfo CSRV_GS_rp_tlm_packet_handler_app(void);

CCP_CmdRet Cmd_CSRV_GS_DRIVER_RESET(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CSRV_GS_SET_RT_FLUSH_INTERVAL(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CSRV_GS_SET_RP_FLUSH_INTERVAL(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CSRV_GS_SET_FARM_PW(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CSRV_GS_SET_INFO(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_CSRV_GS_CCSDS_GET_BUFFER(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CSRV_GS_CCSDS_SET_RATE(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_CSRV_GS_UART_TLM_ON(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CSRV_GS_UART_TLM_OFF(const CommonCmdPacket* packet);

#endif
